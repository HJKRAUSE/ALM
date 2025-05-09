#pragma once

#include "RelinkableHandle.h"
#include "Portfolio.h"
#include "Strategy.h"
#include "Date.h"
#include "Curve.h"

namespace ALM {

    /**
     * @brief A composite strategy that delegates to a buy or sell strategy
     * based on the current cash position.
     *
     * If cash is negative, the `sell_` strategy is applied. Otherwise,
     * the `buy_` strategy is applied.
     */
    class RebalanceStrategy : public Strategy {

    public:
        /**
         * @brief Construct a rebalance strategy with sell and buy components.
         *
         * @param sell Strategy to apply when cash is negative.
         * @param buy Strategy to apply when cash is positive or zero.
         */
        RebalanceStrategy(std::shared_ptr<Strategy> sell, std::shared_ptr<Strategy> buy)
            : sell_(std::move(sell)), buy_(std::move(buy)) {
        }

        /**
         * @brief Applies either the sell or buy strategy based on current cash.
         */
        void apply(
            Portfolio& portfolio,
            double& cash,
            Date step_start,
            Date step_end,
            const std::shared_ptr<const Curve>& curve,
            const std::shared_ptr<TaskExecutor>& executor) override
        {
            if (cash < 0.0) {
                sell_->apply(portfolio, cash, step_start, step_end, curve, executor);
            }
            else {
                buy_->apply(portfolio, cash, step_start, step_end, curve, executor);
            }
        }

    private:
        std::shared_ptr<Strategy> sell_; ///< Strategy to apply when cash < 0
        std::shared_ptr<Strategy> buy_;  ///< Strategy to apply when cash >= 0
    };

}
