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
#include <iostream>
#include <mutex>

#include "Date.h"
#include "Portfolio.h"
#include "Strategy.h"
#include "TaskExecutor.h"
#include "Projection.h"
#include "StartingAssetSolver.h"
#include "Curve.h"
//#include "RelinkableHandle.h"

namespace ALM {

    /**
     * @brief Runs a projection over multiple yield curve scenarios using a shared RelinkableHandle.
     *
     * For each scenario, the curve is relinked, the optimal starting scale is solved,
     * and the projection is run using the current curve.
     */
    class MultiScenarioProjection {
    public:
        /**
         * @brief Constructs the multi-scenario projection engine.
         *
         * @param assets The base asset portfolio used in all scenarios.
         * @param liabilities The liability portfolio used in all scenarios.
         * @param strategy The reinvestment/disinvestment strategy to apply.
         * @param executor The task executor used for parallel pricing.
         * @param curves A vector of yield curves (one per scenario).
         * @param start The projection start date.
         * @param end The projection end date.
         * @param step The projection step frequency (e.g., monthly, annually).
         */
        MultiScenarioProjection(
            Portfolio assets,
            Portfolio liabilities,
            std::shared_ptr<Strategy> strategy,
            std::shared_ptr<TaskExecutor> executor,
            std::vector<std::shared_ptr<Curve>> curves,
            Date start,
            Date end,
            Period step = Period(1, TimeUnit::Months)) : 
            assets_(std::move(assets)),
            liabilities_(std::move(liabilities)),
            strategy_(std::move(strategy)),
            executor_(std::move(executor)),
            curves_(std::move(curves)),
            start_(start),
            end_(end),
            step_(step) {
        }

        /**
         * @brief Runs the projection over all scenarios.
         *
         * For each curve:
         * - The shared handle is relinked
         * - The optimal initial asset scale is solved
         * - A full projection is executed and stored
         *
         * @return A vector of ProjectionResult objects, one per scenario.
         */
        std::vector<ProjectionResult> run() {
            std::vector<std::function<void()>> tasks;

            std::vector<ProjectionResult> results;
            std::mutex results_mutex;

            StartingAssetSolver solver;

            for (size_t i = 0; i < curves_.size(); ++i) {
                auto curve = curves_[i];
                auto task = [&]() {
                    Projection projection(
                        assets_,
                        liabilities_,
                        strategy_,
                        executor_,
                        curve,
                        start_,
                        end_,
                        step_);

                    double scalar = solver.solve(projection);  // Solve for funding level
                    auto result = projection.run(scalar); // Store the result
                    {
                        std::lock_guard<std::mutex> lock(results_mutex);
                        results.push_back(result);
                    }
                };

                tasks.push_back(task);

            }

            executor_->submitAndWait(tasks);

            return results;
        }

    private:
        Portfolio assets_;
        Portfolio liabilities_;
        std::shared_ptr<Strategy> strategy_;
        std::shared_ptr<TaskExecutor> executor_;
        std::vector<std::shared_ptr<Curve>> curves_;
        Date start_;
        Date end_;
        Period step_;
    };

}
