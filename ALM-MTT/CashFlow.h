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

#include "Date.h"

namespace ALM {

    /**
     * @brief Represents a single dated cash flow with an amount.
     *
     * Used to model both asset and liability cash flows in ALM projections.
     */
    struct CashFlow {
        Date date;      ///< The date on which the cash flow occurs
        double amount;  ///< The amount of the cash flow

        /**
         * @brief Checks if the cash flow falls strictly between two dates.
         *
         * @param from The lower bound (exclusive).
         * @param to The upper bound (inclusive).
         * @return true if from < date <= to
         */
        bool occursBetween(const Date& from, const Date& to) const {
            return date > from && date <= to;
        }
    };

}
