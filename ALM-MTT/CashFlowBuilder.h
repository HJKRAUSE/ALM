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
#include "Calendar.h"
#include "Schedule.h"

namespace ALM {

    /**
     * @brief Utility class for generating ALM-compatible cash flows from QuantLib instruments.
     */
    class CashFlowBuilder {
    public:
        /**
         * @brief Creates cash flows for a fixed-rate bond with coupon payments and final principal.
         *
         * @param issue_date Start date of the bond.
         * @param maturity_date Maturity date.
         * @param coupon Fixed annual coupon rate.
         * @param notional Face value of the bond.
         * @param frequency Coupon frequency (default: Semiannual).
         * @param calendar Calendar for scheduling (default: NullCalendar).
         * @param dc Day counter for coupon accrual (default: Actual/Actual(365)).
         * @param bdc Business day convention for date adjustment (default: Unadjusted).
         * @return Vector of ALM::CashFlow representing the bond's payments.
         */
        static std::vector<CashFlow> fixedRateBond(
            Date issue_date,
            Date maturity_date,
            double coupon,
            double notional,
            Duration frequency = Duration(6, Duration::Unit::Months),
            const Calendar& calendar = Calendar(),
            const DayCounter& dc = DayCounter(DayCounter::Convention::ActualActual))
        {
            std::vector<CashFlow> cash_flows;
            Schedule schedule(issue_date, maturity_date, frequency, calendar, true);

            // skip issue date...
            for (auto i = 1; i < schedule.size(); i++) {
                cash_flows.emplace_back(CashFlow(schedule[i], notional * coupon));
            }
            
            cash_flows.emplace_back(CashFlow(calendar.adjust(maturity_date), notional));

            return cash_flows;
        }

        /**
         * @brief Creates a single cash flow for a zero-coupon bond.
         * @param maturity_date Maturity date of the bond.
         * @param face_amount Final repayment amount.
         * @return Vector containing a single ALM::CashFlow.
         */
        static std::vector<CashFlow> zeroCouponBond(
            Date maturity_date,
            double face_amount)
        {
            return { { maturity_date, face_amount } };
        }
    };

}
