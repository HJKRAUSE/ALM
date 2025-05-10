#pragma once

#include <cmath>
#include <stdexcept>
#include <limits>

namespace ALM {

    class BrentSolver {
    public:
        BrentSolver(int max_iter = 100, double tol = 1e-6)
            : max_iter_(max_iter), tol_(tol) {
        }

        double solve(const std::function<double(double)>& f, double lower, double upper, double guess = 0.0) {
            constexpr double eps = std::numeric_limits<double>::epsilon();

            double a = lower, b = upper;
            double fa = f(a), fb = f(b);

            if (fa * fb > 0.0) {
                throw std::invalid_argument("Bracketing failed: f(lower) and f(upper) must have opposite signs.");
            }

            double c = a, fc = fa;
            double d = b - a, e = d;
            double s = b;
            double fs = fb;

            for (int iter = 0; iter < max_iter_; ++iter) {
                if (std::abs(fc) < std::abs(fb)) {
                    a = b; b = c; c = a;
                    fa = fb; fb = fc; fc = fa;
                }

                const double tol1 = 2 * eps * std::abs(b) + 0.5 * tol_;
                const double m = 0.5 * (c - b);

                if (std::abs(m) <= tol1 || fb == 0.0) {
                    return b;
                }

                if (std::abs(e) < tol1 || std::abs(fa) <= std::abs(fb)) {
                    d = e = m;  // bisection
                }
                else {
                    double s;
                    double p, q;
                    if (a == c) {
                        // Secant method
                        s = fb / fa;
                        p = 2 * m * s;
                        q = 1 - s;
                    }
                    else {
                        // Inverse quadratic interpolation
                        double r = fb / fc, s1 = fb / fa;
                        p = s1 * (2 * m * r * (r - s1) - (b - a) * (s1 - 1));
                        q = (r - 1) * (s1 - 1) * (r - s1);
                    }

                    if (p > 0) q = -q;
                    p = std::abs(p);

                    if (2 * p < std::min(3 * m * q - std::abs(tol1 * q), std::abs(e * q))) {
                        e = d;
                        d = p / q;
                    }
                    else {
                        d = e = m;
                    }
                }

                a = b;
                fa = fb;
                if (std::abs(d) > tol1)
                    b += d;
                else
                    b += (m > 0 ? tol1 : -tol1);
                fb = f(b);

                if ((fb > 0 && fc > 0) || (fb < 0 && fc < 0)) {
                    c = a;
                    fc = fa;
                    d = e = b - a;
                }
            }

            return b;  // failed to converge
        }

    private:
        int max_iter_;
        double tol_;
    };

}
