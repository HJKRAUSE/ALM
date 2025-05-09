#pragma once

#include "Projection.h"
#include <ql/math/solvers1d/brent.hpp>

namespace ALM {

    /**
     * @brief Solver for determining the portfolio scaling factor that achieves a target surplus.
     *
     * Uses QuantLib's Brent solver to find the scalar multiplier on asset volume such that
     * the final projected surplus is approximately zero (or within a specified tolerance).
     */
    class StartingAssetSolver {
    public:
        /**
         * @brief Solves for the asset scale factor that zeroes out the final surplus.
         *
         * @param projection The projection to evaluate (will be called repeatedly).
         * @param max_evaluations Maximum number of solver iterations.
         * @param tolerance Absolute tolerance for the surplus target.
         * @param guess Initial guess for the scaling factor.
         * @param lower_bound Lower bound of search interval.
         * @param upper_bound Upper bound of search interval.
         * @return Scaling factor such that projection.run(scale).ending_surplus ≈ 0.
         */
        double solve(
            Projection& projection,
            int max_evaluations = 1000,
            double tolerance = 1e-6,
            double guess = 1.0,
            double lower_bound = 0.0,
            double upper_bound = 100.0)
        {
            QuantLib::Brent solver;
            solver.setMaxEvaluations(max_evaluations);

            auto f = [&](double scalar) {
                auto x = projection.run(scalar).ending_surplus;
                return x;
                };

            return solver.solve(f, tolerance, guess, lower_bound, upper_bound);
        }
    };

}
