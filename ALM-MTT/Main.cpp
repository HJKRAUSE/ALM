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

using namespace QuantLib;
using namespace ALM;

int main() {
    std::cout << "ALM Optimization Test\nCopyright (c) 2025 Harold James Krause\n" << std::endl;



    std::cout 
        << "******************************************************\n\n" 
        << "Configuring..." 
        << std::endl;

    Date today(7, May, 2025);
    Settings::instance().evaluationDate() = today;

    std::cout << "Multithreading:\tstarted";
    auto executor = std::make_shared<MultiThreadedExecutor>();
    std::cout << " | ended" << std::endl;

    std::cout  << "Yield curves:\tstarted";
    std::vector<std::shared_ptr<YieldTermStructure>> curves;
    auto make_flat_curve = [&](Rate r) {
        return std::make_shared<FlatForward>(today, Handle<Quote>(std::make_shared<SimpleQuote>(r)), Actual365Fixed());
        };
    for (auto i = 0; i < 9; i++) {
        curves.push_back(make_flat_curve(0.01 * i + 0.03));
    }
    std::cout << " | ended" << std::endl;

    std::cout << "Assets:\t\tstarted";
    Portfolio assetPortfolio;
    for (int i = 0; i < 10; ++i) {
        double coupon = 0.03 + 0.001 * i;
        Date maturity = today + Period((i + 1)*2, Years);
        auto cfs = CashFlowBuilder::fixedRateBond(today, maturity, coupon, 1000.0);
        assetPortfolio.addAsset(Asset(std::move(cfs)));
    }
    std::cout << " | ended" << std::endl;

    std::cout << "Liabilities:\tstarted";
    // 4. Liability portfolio: 10 annual payouts of 5000
    Portfolio liabilityPortfolio;
    for (int i = 1; i <= 30; ++i) {
        Date liabDate = today + Period(i, Years);
        liabilityPortfolio.addAsset(Asset({ { liabDate, 1000.0 } }));
    }
    std::cout << " | ended" << std::endl;

    std::cout << "Strategies:\tstarted";
    // 5. Strategy: sell pro-rata + reinvest into 10Y bonds at 4.5%
    auto sell = std::make_shared<SellProRata>();
    auto buy = std::make_shared<BuyBonds>(std::vector<BuyBonds::BondTemplate>{
        {1.0, 0.045, Period(5, Years)}
    });
    auto strategy = std::make_shared<RebalanceStrategy>(sell, buy);
    std::cout << " | ended" << std::endl;

    std::cout << "Solver:\t\tstarted";
    size_t n_assets = assetPortfolio.assets().size();
    Eigen::VectorXd x0 = Eigen::VectorXd::Constant(n_assets, 1.0);
    Eigen::VectorXd lower = Eigen::VectorXd::Constant(n_assets, 0.0);
    Eigen::VectorXd upper = Eigen::VectorXd::Constant(n_assets, 1.0);


    std::vector<std::shared_ptr<ALM::Constraint>> constraints = {
        std::make_shared<ALM::BoxConstraint>(lower, upper)
    };

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

    ProjectedGradientSolver solver(constraints, 20);
    std::cout << " | ended\n" << std::endl;

    std::cout << "******************************************************\n\n"
        << "Solving...\n"
        << "Length:\t\t10 Years\n"
        << "Time step:\tAnnual\n"
        << "Scenarios:\t9\n"
        << "Iterations:\t20\n"
        << std::endl;

    SolverResults result = solver.solve(f, x0);

    std::cout << "\n******************************************************\n\n";

    std::cout << "Asset Market Value:\t" << std::setprecision(2) << std::fixed << result.objective << std::endl;
    std::cout << "Asset Scalars:\t\t[" << std::setprecision(2) << std::fixed  << result.x.transpose() << "]" << std::endl;
    std::cout << "\n";

    return 0;
}
