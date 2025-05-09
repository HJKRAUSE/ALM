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

#include <vector>
#include "Date.h"
#include "CashFlow.h"
#include "Curve.h"

namespace ALM {

    /**
     * @brief Represents a financial asset as a set of projected cash flows and a volume scalar.
     *
     * The asset can be priced against a yield curve, and its cash flows summed over a date range.
     */
    class Asset {

    public:
        /**
         * @brief Construct an asset from a set of cash flows and an optional volume scalar.
         * @param cash_flows The cash flows (in original volume units).
         * @param volume The scalar (e.g., number of units or par) applied to cash flows.
         */
        Asset(std::vector<CashFlow> cash_flows, double volume = 1.0)
            : cash_flows_(std::move(cash_flows)), volume_(volume) {
        }

        /**
         * @brief Calculate the market value of the asset using the given curve and reference date.
         * @param curve The yield curve used to discount future cash flows.
         * @param ref The reference date for pricing.
         * @return Present value of future cash flows after the reference date, scaled by volume.
         */
        double marketValue(const std::shared_ptr<const Curve>& curve, const Date& ref) const {
            double total = 0.0;
            for (const auto& cf : cash_flows_) {
                if (cf.date >= ref) {
                    double df = curve->discount(cf.date);
                    total += cf.amount * df;
                }
            }
            return total * volume_;
        }

        /**
         * @brief Calculate the total cash flow within a specified date range.
         * @param from Start date (exclusive).
         * @param to End date (inclusive).
         * @return Sum of applicable cash flows scaled by volume.
         */
        double cashFlow(const Date& from, const Date& to) const {
            double total = 0.0;
            for (const auto& cf : cash_flows_) {
                if (cf.occursBetween(from, to)) {
                    total += cf.amount;
                }
            }
            return total * volume_;
        }

        /// Set the asset volume multiplier
        void setVolume(double volume) {
            volume_ = volume;
        }

        /// Get the asset volume multiplier
        double volume() const {
            return volume_;
        }

    private:
        std::vector<ALM::CashFlow> cash_flows_;  ///< Immutable list of original cash flows
        double volume_;                          ///< Scalar multiplier applied to cash flows
    };

}
