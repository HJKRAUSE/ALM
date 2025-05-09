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
#include "GradientSolver.h"
#include "Constraint.h"
#include "BoxConstraint.h"

using namespace QuantLib;
using namespace ALM;

int main() {
    
    Date today(7, May, 2025);
    Settings::instance().evaluationDate() = today;

    // 1. Create multiple yield curves (base + stress)
    std::vector<std::shared_ptr<YieldTermStructure>> curves;

    auto make_flat_curve = [&](Rate r) {
        return std::make_shared<FlatForward>(today, Handle<Quote>(std::make_shared<SimpleQuote>(r)), Actual365Fixed());
        };

    for (auto i = 0; i < 9; i++) {
        curves.push_back(make_flat_curve(0.01 * i + 0.03));
    }

    // 2. Task executor
    auto executor = std::make_shared<SingleThreadedExecutor>();

    // 3. Asset portfolio: 10 fixed-rate bonds with increasing coupons
    Portfolio assetPortfolio;
    for (int i = 0; i < 5; ++i) {
        double coupon = 0.03 + 0.001 * i;
        Date maturity = today + Period(10, Years);
        auto cfs = CashFlowBuilder::fixedRateBond(today, maturity, coupon, 1000.0);
        assetPortfolio.addAsset(Asset(std::move(cfs)));
    }

    // 4. Liability portfolio: 10 annual payouts of 5000
    Portfolio liabilityPortfolio;
    for (int i = 1; i <= 10; ++i) {
        Date liabDate = today + Period(i, Years);
        liabilityPortfolio.addAsset(Asset({ { liabDate, 5000.0 } }));
    }

    // 5. Strategy: sell pro-rata + reinvest into 10Y bonds at 4.5%
    auto sell = std::make_shared<SellProRata>();
    auto buy = std::make_shared<BuyBonds>(std::vector<BuyBonds::BondTemplate>{
        {1.0, 0.045, Period(10, Years)}
    });
    auto strategy = std::make_shared<RebalanceStrategy>(sell, buy);

    Eigen::VectorXd x0 = Eigen::VectorXd::Constant(5, 1.0);
    Eigen::VectorXd lower(5);
    lower << 0, 0, 0, 0, 0;
    Eigen::VectorXd upper(5);
    upper << 1, 1, 1, 1, 100;

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

    ProjectedGradientSolver solver(constraints, 1000);
    SolverResults result = solver.solve(f, x0);

    std::cout << result.x << std::endl;
    return 0;
}
