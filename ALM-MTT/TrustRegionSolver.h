#pragma once
#include <Eigen/Dense>
#include <functional>
#include <vector>
#include <memory>
#include "Solver.h"
#include "Constraint.h"
#include "UI.h"

namespace ALM {

    class TrustRegionSolver : public Solver {
    public:
        TrustRegionSolver(
            std::vector<std::shared_ptr<Constraint>> constraints = {},
            int max_iterations = 100,
            double initial_radius = 1.0,
            double eta = 0.1,
            double tolerance = 1e-4)
            : constraints_(std::move(constraints)),
            max_iter_(max_iterations),
            delta_(initial_radius),
            eta_(eta),
            tol_(tolerance) {
        }

        SolverResults solve(const std::function<double(Eigen::VectorXd)>& f,
            const Eigen::VectorXd& x0) override {

            Eigen::VectorXd x = x0;
            double fx = f(x);
            int n = x.size();

            for (int iter = 0; iter < max_iter_; ++iter) {
                Eigen::VectorXd grad;
                Eigen::MatrixXd hess;
                computeGradientAndHessian(f, x, fx, grad, hess);

                double grad_norm = grad.norm();
                if (grad_norm < tol_) {
                    return { x, fx, iter + 1, true };
                }

                Eigen::VectorXd p = doglegStep(grad, hess);

                Eigen::VectorXd x_trial = x + p;
                for (const auto& constraint : constraints_)
                    constraint->project(x_trial);

                double fx_trial = f(x_trial);
                double predicted_reduction = -grad.dot(p) - 0.5 * p.dot(hess * p);
                double actual_reduction = fx - fx_trial;

                double rho = actual_reduction / (predicted_reduction + 1e-8);

                if (rho > eta_) {
                    x = x_trial;
                    fx = fx_trial;
                }

                delta_ *= adjustRadius(rho);

                UI::debugPrint("Iter " + std::to_string(iter) + ", fx = " + std::to_string(fx) + ", radius = " + std::to_string(delta_));
            }

            return { x, fx, max_iter_, false };
        }

    private:
        std::vector<std::shared_ptr<Constraint>> constraints_;
        int max_iter_;
        double delta_;
        double eta_;
        double tol_;

        void computeGradientAndHessian(const std::function<double(Eigen::VectorXd)>& f,
            const Eigen::VectorXd& x,
            double fx,
            Eigen::VectorXd& grad,
            Eigen::MatrixXd& hess) {
            double eps = 1e-6;
            int n = x.size();
            grad = Eigen::VectorXd(n);
            hess = Eigen::MatrixXd(n, n);

            for (int i = 0; i < n; ++i) {
                Eigen::VectorXd x1 = x;
                x1[i] += eps;
                double f1 = f(x1);
                grad[i] = (f1 - fx) / eps;
            }

            for (int i = 0; i < n; ++i) {
                for (int j = i; j < n; ++j) {
                    Eigen::VectorXd x1 = x;
                    x1[i] += eps;
                    x1[j] += eps;
                    double fpp = f(x1);

                    x1 = x;
                    x1[i] += eps;
                    double fp = f(x1);

                    x1 = x;
                    x1[j] += eps;
                    double fq = f(x1);

                    double hij = (fpp - fp - fq + fx) / (eps * eps);
                    hess(i, j) = hij;
                    hess(j, i) = hij;
                }
            }
        }

        Eigen::VectorXd doglegStep(const Eigen::VectorXd& grad, const Eigen::MatrixXd& hess) {
            Eigen::VectorXd p_b;
            Eigen::VectorXd p_u = -(grad.dot(grad) / grad.dot(hess * grad)) * grad;

            Eigen::VectorXd p_n = -hess.ldlt().solve(grad);

            if (p_n.norm() <= delta_) {
                return p_n; // full Newton step
            }
            else if (p_u.norm() >= delta_) {
                return (delta_ / p_u.norm()) * p_u; // scaled steepest descent
            }
            else {
                Eigen::VectorXd p_diff = p_n - p_u;
                double a = p_diff.squaredNorm();
                double b = 2.0 * p_u.dot(p_diff);
                double c = p_u.squaredNorm() - delta_ * delta_;
                double tau = (-b + std::sqrt(b * b - 4 * a * c)) / (2 * a);
                return p_u + tau * (p_n - p_u); // blended step
            }
        }

        double adjustRadius(double rho) const {
            if (rho < 0.25)
                return 0.25;
            else if (rho > 0.75 && delta_ < 10.0)
                return 2.0;
            return 1.0;
        }
    };

}
