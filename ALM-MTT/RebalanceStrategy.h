/*
    MIT License

    Copyright (c) 2025 Harold James Krause

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

#pragma once

#include "RelinkableHandle.h"
#include "Portfolio.h"
#include "Strategy.h"
#include "Date.h"
#include "YieldCurve.h"

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
            const std::shared_ptr<const YieldCurve>& curve) override
        {
            if (cash < 0.0) {
                sell_->apply(portfolio, cash, step_start, step_end, curve);
            }
            else {
                buy_->apply(portfolio, cash, step_start, step_end, curve);
            }
        }

    private:
        std::shared_ptr<Strategy> sell_; ///< Strategy to apply when cash < 0
        std::shared_ptr<Strategy> buy_;  ///< Strategy to apply when cash >= 0
    };

}
