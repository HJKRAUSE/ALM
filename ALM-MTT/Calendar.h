#pragma once

#include <vector>
#include <algorithm>
#include "Date.h"

namespace ALM {
	
	class Calendar {
	public:
		enum class Convention {
			Unadjusted,
			Following,
			ModifiedFollowing,
			Preceding,
			ModifiedPreceding,
		};

		// static Calendar UnitedStates(Convention convention = Convention::ModifiedFollowing) {
		//	return Calendar();
		// }

		Calendar(std::vector<Date> holidays = {}, Convention convention = Convention::ModifiedFollowing) :
			holidays_(std::move(holidays)), convention_(convention) {
			std::sort(holidays_.begin(), holidays_.end());
		}

		bool isWeekend(const Date& d) const {
			auto wd = d.weekday();
			return wd == Weekday::Sunday || wd == Weekday::Saturday;
		}

		bool isHoliday(const Date& d) const {
			return std::binary_search(holidays_.begin(), holidays_.end(), d);
		}

		bool isBusinessDay(const Date& d) const {
			return !isWeekend(d) && !isHoliday(d);
		}

		void addHoliday(const Date& d) {
			holidays_.push_back(d);
			std::sort(holidays_.begin(), holidays_.end());
		}

		void addHolidays(const std::vector<Date>& holidays) {
			holidays_.insert(holidays_.end(), holidays.begin(), holidays.end());
			std::sort(holidays_.begin(), holidays_.end());
		}

		Date advance(const Date& d, const Duration& dur) const {
			return adjust(d + dur);
		}

		Date adjust(const Date& d) const {
			Date adj = d;
			switch (convention_) {
			case Convention::Following:

				// Forward seek the next business day
				while (!isBusinessDay(adj)) {
					adj += Duration(1, Duration::Unit::Days);
				}
				return adj;

			case Convention::ModifiedFollowing:

				// Forward seek the next business day...
				// unless you end up in the next month
				// then backward seek
				while (!isBusinessDay(adj)) { //fwd search
					adj += Duration(1, Duration::Unit::Days);
				}
				if (adj.month() == d.month()) { // okay to go
					return adj;
				}
				adj = d;
				while (!isBusinessDay(adj)) { // need to back search
					adj -= Duration(1, Duration::Unit::Days); // can start with d safely
				}
				return adj;

			case Convention::Preceding:

				// Backward seek the next business day
				while (!isBusinessDay(adj)) {
					adj -= Duration(1, Duration::Unit::Days);
				}
				return adj;

			case Convention::ModifiedPreceding:

				// Backward seek the next business day...
				// unless you end up in the previous month
				// then forward seek
				while (!isBusinessDay(adj)) {
					adj -= Duration(1, Duration::Unit::Days);
				}
				if (adj.month() == d.month()) { // okay to go
					return adj;
				}
				adj = d;
				while (!isBusinessDay(adj)) { // need to back search
					adj += Duration(1, Duration::Unit::Days); // can start with d safely
				}
				return adj;

			case Convention::Unadjusted:
			default:
				return d;
			}
		}

	private:
		Convention convention_;
		std::vector<Date> holidays_;
	};
}