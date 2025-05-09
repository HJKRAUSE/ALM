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
#include <Eigen/dense>
#include "Solver.h"
#include "Constraint.h"
#include "UI.h"

namespace ALM {

    class ProjectedGradientSolver : public Solver {
    public:
        ProjectedGradientSolver(
            std::vector<std::shared_ptr<Constraint>> constraints = {},
            int max_iterations = 100,
            double step_size = 1e-2,
            double tolerance = 1e-4)
            :
            constraints_(std::move(constraints)),
            max_iter_(max_iterations),
            alpha_(step_size),
            tol_(tolerance) {
        }

        SolverResults solve(const std::function<double(Eigen::VectorXd)>& f,
            const Eigen::VectorXd& x0) override {

            Eigen::VectorXd x = x0;
            double fx = f(x);
            int n = x.size();

            for (int iter = 0; iter < max_iter_; ++iter) {

                Eigen::VectorXd grad(n);
                double eps = 1e-6;

                for (int i = 0; i < n; ++i) {
                    Eigen::VectorXd x_perturbed = x;
                    x_perturbed[i] += eps;
                    grad[i] = (f(x_perturbed) - fx) / eps;
                }

                // Gradient step
                x -= alpha_ * grad;

                // Project onto all constraints
                for (const auto& constraint : constraints_) {
                    constraint->project(x);
                }

                double fx_new = f(x);
                if (std::abs(fx_new - fx) < tol_) {
                    return { x, fx_new, iter + 1, true };
                }

                fx = fx_new;
                UI::debugPrint("Iteration " + std::to_string(iter) + ": " + std::to_string(fx));
            }

            return { x, fx, max_iter_, false };
        }

    private:
        std::vector<std::shared_ptr<Constraint>> constraints_;
        int max_iter_;
        double alpha_;
        double tol_;
    };
}