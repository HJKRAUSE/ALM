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
#include "CashFlowBuilder.h"
#include "Date.h"
#include "YieldCurve.h"

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
            Date step_start,
            Date step_end,
            const std::shared_ptr<const YieldCurve>& curve) = 0;
    };

}
