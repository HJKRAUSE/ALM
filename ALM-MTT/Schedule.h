#pragma once

#include "Date.h"
#include "Calendar.h"

namespace ALM {
	class Schedule {
	public:
		Schedule(
			Date start,
			Date end,
			Duration step,
			const Calendar& calendar = Calendar(),
			bool include_end = true)  
		{ 
			generate(start, end, step, calendar, include_end);
		}

		const std::vector<Date>& dates() const {
			return dates_;
		}
		size_t size() const { return dates_.size(); }

		const Date& operator[](size_t i) {
			return dates_[i];
		}

	private:
		std::vector<Date> dates_;

		void generate(
			Date start,
			Date end,
			Duration step,
			const Calendar& calendar,
			bool include_end) {

			Date current = calendar.adjust(start);
			dates_.push_back(current);

			while (true) {
				Date next = calendar.advance(current, step);
				if (next > end) break;
				dates_.push_back(next);
				current = next;
			}

			if (include_end || dates_.empty()) {
				Date end_date = calendar.adjust(end);
				if(dates_.back() < end_date || dates_.back() != end_date)
				dates_.push_back(end_date);
			}
		}
	};
}