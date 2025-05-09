#pragma once

#include <ql/quantlib.hpp>
#include <vector>
#include "Date.h"
#include "CashFlow.h"

namespace ALM {

    /**
     * @brief Utility class for generating ALM-compatible cash flows from QuantLib instruments.
     */
    class CashFlowBuilder {
    public:
        /**
         * @brief Converts a QuantLib Leg to a vector of ALM::CashFlow.
         * @param leg The QuantLib Leg (vector of shared_ptr<CashFlow>).
         * @return A flat vector of ALM::CashFlow with date and amount extracted.
         */
        static std::vector<CashFlow> fromLeg(const QuantLib::Leg& leg) {
            std::vector<CashFlow> out;
            out.reserve(leg.size());
            for (const auto& cf : leg) {
                out.push_back({ cf->date(), cf->amount() });
            }
            return out;
        }

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
            QuantLib::Frequency frequency = QuantLib::Semiannual,
            const QuantLib::Calendar& calendar = QuantLib::NullCalendar(),
            const QuantLib::DayCounter& dc = QuantLib::ActualActual(QuantLib::ActualActual::Actual365),
            const QuantLib::BusinessDayConvention& bdc = QuantLib::Unadjusted)
        {
            using namespace QuantLib;

            Schedule schedule(issue_date, maturity_date, Period(frequency),
                calendar, bdc, bdc, DateGeneration::Forward, false);

            Leg leg = FixedRateLeg(schedule)
                .withCouponRates(coupon, dc)
                .withNotionals(notional);

            // Add principal repayment at maturity
            leg.push_back(std::make_shared<SimpleCashFlow>(notional, maturity_date));

            return fromLeg(leg);
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
