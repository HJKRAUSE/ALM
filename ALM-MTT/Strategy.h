#pragma once

#include "RelinkableHandle.h"
#include <ql/quantlib.hpp>
#include "Portfolio.h"
#include "CashFlowBuilder.h"
#include "Date.h"
#include "Curve.h"

namespace ALM {

    /**
     * @brief Abstract base class for reinvestment/disinvestment strategies in ALM projections.
     *
     * A strategy is applied at each projection step and can modify the portfolio and cash balance.
     * Implementations may choose to buy, sell, or hold assets based on current state.
     */
    class Strategy {

    protected:
        Strategy() = default;

    public:
        virtual ~Strategy() = default;

        /**
         * @brief Apply the strategy for the current time step.
         *
         * @param portfolio The portfolio to be adjusted.
         * @param cash Current cash available (can be negative for shortfall).
         * @param step_start Start of the projection period.
         * @param step_end End of the projection period.
         * @param curve Yield curve used for pricing or reinvestment logic.
         * @param executor Task executor for parallel portfolio operations.
         */
        virtual void apply(
            Portfolio& portfolio,
            double& cash,
            QuantLib::Date step_start,
            QuantLib::Date step_end,
            const std::shared_ptr<const Curve>& curve,
            const std::shared_ptr<TaskExecutor>& executor) = 0;
    };

}
