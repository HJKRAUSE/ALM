#pragma once
#include <Eigen/dense>
#include "Solver.h"
#include "Constraint.h"

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