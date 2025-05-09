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
