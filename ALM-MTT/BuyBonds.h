#pragma once

#include "RelinkableHandle.h"
#include <ql/quantlib.hpp>
#include "Portfolio.h"
#include "CashFlowBuilder.h"
#include "Date.h"
#include "Strategy.h"

namespace ALM {

    /**
     * @brief Strategy that reinvests available cash into fixed-rate bonds using predefined templates.
     *
     * Bonds are purchased in proportions specified by the strategy. Each template defines the
     * percentage of available cash to use, the coupon rate, and the bond tenor.
     */
    class BuyBonds : public Strategy {
        using CurveHandle = RelinkableHandle<QuantLib::YieldTermStructure>;

    public:
        /**
         * @brief Structure defining a bond reinvestment template.
         *
         * Each entry specifies a proportion of available cash, a fixed coupon, and a tenor.
         */
        struct BondTemplate {
            double proportion;       ///< Fraction of available cash to allocate (e.g. 0.25 = 25%)
            double coupon;           ///< Fixed coupon rate (annual)
            QuantLib::Period tenor;  ///< Bond tenor (e.g. 5Y, 10Y)
        };

        /**
         * @brief Construct the BuyBonds strategy with a list of templates.
         */
        BuyBonds(std::vector<BondTemplate> templates)
            : templates_(std::move(templates)) {
        }

        /**
         * @brief Applies the strategy by reinvesting cash into fixed-rate bonds.
         */
        void apply(
            Portfolio& portfolio,
            double& cash,
            Date step_start,
            Date step_end,
            const std::shared_ptr<const Curve>& curve,
            const std::shared_ptr<TaskExecutor>& executor) override
        {
            if (cash <= 0.0)
                return;

            for (const auto& bond_template : templates_) {
                double amount = cash * bond_template.proportion;
                if (amount < 1e-6) continue;  // Skip tiny allocations

                Asset bond = Asset(CashFlowBuilder::fixedRateBond(
                    step_start,
                    step_start + bond_template.tenor,
                    bond_template.coupon,
                    amount));

                portfolio.addAsset(std::move(bond));
                cash -= amount;
            }

            // Optional: clamp to 0 in case of floating point underflow
            if (cash < 1e-6) cash = 0.0;
        }

    private:
        std::vector<BondTemplate> templates_;  ///< List of bond reinvestment targets
    };

}
