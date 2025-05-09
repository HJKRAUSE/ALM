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
