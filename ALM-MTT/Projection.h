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

#include <ql/quantlib.hpp>
#include <vector>
#include <memory>
#include "Date.h"
#include "Portfolio.h"
#include "RelinkableHandle.h"
#include "Curve.h"
#include "TaskExecutor.h"
#include "Strategy.h"

namespace ALM {

    /**
     * @brief Stores results of a projection over time.
     *
     * Tracks key metrics per time step: dates, asset/liability values, cash, and surplus.
     */
    struct ProjectionResult {
        double scalar;
        std::vector<Date> dates;
        std::vector<double> assets_bop;
        std::vector<double> liabilities_bop;
        std::vector<double> cash_bop;
        std::vector<double> surplus_bop;
        double ending_surplus = 0.0;
    };

    /**
     * @brief Runs a forward ALM projection with asset, liability, and strategy logic.
     *
     * Supports parallel pricing through a task executor and uses a strategy for reinvestment/disinvestment.
     */
    class Projection {
    public:
        /**
         * @brief Construct a projection object.
         *
         * @param assets The starting asset portfolio.
         * @param liabilities The projected liabilities as a portfolio.
         * @param strategy The strategy to apply at each time step (buy/sell).
         * @param executor Threaded or single-threaded task executor.
         * @param curve The yield curve used for pricing and discounting.
         * @param start The start date of the projection.
         * @param end The end date of the projection.
         * @param step The interval between time steps (e.g., 1Y, 1M).
         */
        Projection(
            Portfolio assets,
            Portfolio liabilities,
            std::shared_ptr<Strategy> strategy,
            std::shared_ptr<TaskExecutor> executor,
            std::shared_ptr<Curve> curve,
            Date start,
            Date end,
            Period step = Period(1, TimeUnit::Months))
            : assets_(std::move(assets)),
            liabilities_(std::move(liabilities)),
            strategy_(std::move(strategy)),
            executor_(std::move(executor)),
            curve_(std::move(curve)),
            start_(start),
            end_(end),
            step_(step) { }

        /**
         * @brief Runs the projection for a given initial asset scalar.
         *
         * @param scalar Multiplier to apply to starting asset volumes.
         * @return ProjectionResult containing time series and final surplus.
         */
        ProjectionResult run(double scalar = 1.0) {
            ProjectionResult result;
            result.scalar = scalar;

            Portfolio portfolio = assets_;  // Copy assets to allow modification
            for (auto& asset : portfolio.assets()) {
                asset.setVolume(asset.volume() * scalar);
            }

            double cash = 0.0;
            Date current = start_;
            Date next = current + step_;

            while (current < end_) {
                // Record date
                result.dates.push_back(current);

                // Asset and liability valuation at beginning of period
                double mv = portfolio.marketValue(curve_, current, executor_);
                double liability_mv = liabilities_.marketValue(curve_, current, executor_);

                result.assets_bop.push_back(mv);
                result.liabilities_bop.push_back(liability_mv);
                result.cash_bop.push_back(cash);
                result.surplus_bop.push_back(mv + cash - liability_mv);

                // Asset inflows and liability outflows
                double asset_cf = portfolio.cashFlow(current, next, executor_);
                double liability_cf = liabilities_.cashFlow(current, next, executor_);

                cash += asset_cf - liability_cf;

                // Apply strategy logic
                if (strategy_) {
                    strategy_->apply(portfolio, cash, current, next, curve_, executor_);
                }

                current = next;
                next = current + step_;
            }

            // Compute final surplus (BOP assets + ending cash - final liability BOP)
            result.ending_surplus = result.assets_bop.back() + cash - result.liabilities_bop.back();
            return result;
        }

    private:
        Portfolio assets_;
        Portfolio liabilities_;
        std::shared_ptr<Strategy> strategy_;
        std::shared_ptr<TaskExecutor> executor_;
        std::shared_ptr<Curve> curve_;
        Date start_;
        Date end_;
        Period step_;
    };

}
