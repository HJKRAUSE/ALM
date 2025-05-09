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

#include <iostream>
#include <memory>
#include <ql/quantlib.hpp>

#include "Date.h"
#include "Asset.h"
#include "Portfolio.h"
#include "CashFlowBuilder.h"
#include "TaskExecutor.h"
#include "MultiThreadedExecutor.h"
#include "SingleThreadedExecutor.h"
#include "Strategy.h"
#include "BuyBonds.h"
#include "SellProRata.h"
#include "RebalanceStrategy.h"
#include "MultiScenarioProjection.h"
#include "ProjectedGradientSolver.h"
#include "Constraint.h"
#include "BoxConstraint.h"
#include "UI.h"
#include "TrustRegionSolver.h"

using namespace QuantLib;
using namespace ALM;

int main() {
    UI::useColor();
    UI::setVerbosity(UI::Verbosity::Debug);

    UI::section("ALM Optimization Test");
    UI::print("ALM Optimization Test\nCopyright (c) 2025 Harold James Krause\n");

    UI::section("Config");

    Date today(7, May, 2025);
    Settings::instance().evaluationDate() = today;

    bool use_mtt = UI::askYesNo("Use multithreading?");

    std::shared_ptr<TaskExecutor> executor;
    if (use_mtt) {
        executor = std::make_shared<MultiThreadedExecutor>();
        UI::print("Multi-threading configuration complete");
        UI::debugPrint("Initialized MultiThreadedExecutor with default limits");
    } else {
        executor = std::make_shared<SingleThreadedExecutor>();
        UI::print("Single-threading configuration complete");
        UI::debugPrint("Initialized SingleThreadedExecutor");
        UI::warn("Single-threading not recommended for complex projections");
    }

    std::vector<std::shared_ptr<YieldTermStructure>> curves;
    auto make_flat_curve = [&](Rate r) {
        return std::make_shared<FlatForward>(today, Handle<Quote>(std::make_shared<SimpleQuote>(r)), Actual365Fixed());
        };
    for (auto i = 0; i < 9; i++) {
        curves.push_back(make_flat_curve(0.01 * i + 0.03));
    }
    
    UI::print("Initialized scenario count: 9");
    UI::debugPrint("FlatForward with annual compounded rate: 0.01i + 0.03");

    Portfolio assetPortfolio;
    for (int i = 0; i < 10; ++i) {
        double coupon = 0.03 + 0.001 * i;
        Date maturity = today + Period((i + 1)*2, Years);
        auto cfs = CashFlowBuilder::fixedRateBond(today, maturity, coupon, 1000.0);
        assetPortfolio.addAsset(Asset(std::move(cfs)));
    }
    
    UI::print("Inforce asset count: 10");
    UI::debugPrint("FixedRateBond with maturity: 2(i+1) Years");
    UI::debugPrint("FixedRateBond with coupon rate: 0.001i + 0.03");

    // 4. Liability portfolio: 10 annual payouts of 5000
    Portfolio liabilityPortfolio;
    for (int i = 1; i <= 30; ++i) {
        Date liabDate = today + Period(i, Years);
        liabilityPortfolio.addAsset(Asset({ { liabDate, 1000.0 } }));
    }
    UI::print("Liability cash flow count: 30");
    UI::debugPrint("Fixed annual cash flows of 1000");

    // 5. Strategy: sell pro-rata + reinvest into 10Y bonds at 4.5%
    auto sell = std::make_shared<SellProRata>();
    UI::print("Disinvestment strategy initialized");
    UI::debugPrint("Sell pro-rata");

    auto buy = std::make_shared<BuyBonds>(std::vector<BuyBonds::BondTemplate>{
        {1.0, 0.045, Period(5, Years)}
    });
    UI::print("Reinvestment strategy initialized");
    UI::debugPrint("Buy 5Y bonds yielding 4.5%");

    auto strategy = std::make_shared<RebalanceStrategy>(sell, buy);


    size_t n_assets = assetPortfolio.assets().size();
    Eigen::VectorXd x0 = Eigen::VectorXd::Constant(n_assets, 1.0);
    Eigen::VectorXd lower = Eigen::VectorXd::Constant(n_assets, 0.0);
    Eigen::VectorXd upper = Eigen::VectorXd::Constant(n_assets, 1.0);

    std::vector<std::shared_ptr<ALM::Constraint>> constraints = {
        std::make_shared<ALM::BoxConstraint>(lower, upper)
    };

    UI::print("Solver constraints initialized");
    UI::debugPrint("X E [0, 1]");

    auto f = [&](const Eigen::VectorXd& x) {
        Portfolio portfolio = assetPortfolio;
        for (auto i = 0; i < x.size(); i++) {
            portfolio.assets()[i].setVolume(x[i]);
        }
        
        MultiScenarioProjection runner(
            portfolio,
            liabilityPortfolio,
            strategy,
            executor,
            curves,
            today,
            today + Period(10, Years),
            Period(1, Years)
        );

        auto results = runner.run();

        double tmp = 0;
        for (const auto& result : results) {
            tmp = std::max(tmp, result.assets_bop[0]);
        }

        return tmp;

        };

    UI::print("Solver lambda initialized");
    UI::debugPrint("Max solved-for assets across each scenario");

    TrustRegionSolver solver(constraints, 12);
    UI::print("Trust region solver initialized");
    UI::debugPrint("Dogleg subproblem");
    UI::debugPrint("Max iterations: 12");
    if (use_mtt) {
        UI::warn("Gradients and hessians are not parallelized");
    }

    UI::section("Solver");
    UI::print("Begin solving lambda");
    SolverResults result = solver.solve(f, x0);

    UI::print("End solving lambda");
    result.success ? UI::print("Solver successfully converged") : UI::warn("Solver failed to converge");

    std::cout << "Asset Market Value:\t" << std::setprecision(2) << std::fixed << result.objective << std::endl;
    std::cout << "Asset Scalars:\t\t[" << std::setprecision(2) << std::fixed  << result.x.transpose() << "]" << std::endl;
    std::cout << "\n";

    return 0;
    
}
