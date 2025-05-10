#pragma once

#include "Date.h"

namespace ALM {

	class DayCounter {
	public:
		enum class Convention {
			ActualActual,
			Actual365,
			Thirty360
		};

		DayCounter(Convention convention) : convention_(convention) { } 
		double yearFraction(const Date& start, const Date& end) const {
			switch (convention_) {
			case Convention::ActualActual:
				return actualActual(start, end);
			case Convention::Actual365:
				return actual365(start, end);
			case Convention::Thirty360:
				return thirty360(start, end);
			}
		}
		virtual int dayCount(const Date& start, const Date& end) const {
			return end.serial() - start.serial();
		}
	private:
		Convention convention_;

		double actualActual(const Date& start, const Date& end) const {
			int y1 = start.year(), y2 = end.year();
			if (y1 == y2) {
				double denom = Date::isLeapYear(y1) ? 366.0 : 365.0;
				return static_cast<double>(dayCount(start, end)) / denom;
			}
			else {
				Date endOfY1 = Date::YMDToSerial({ y1 + 1, 1, 1 });
				Date startOfY2 = Date::YMDToSerial({ y2, 1, 1 });
				double f1 = static_cast<double>(dayCount(start, endOfY1)) / (Date::isLeapYear(y1) ? 366.0 : 365.0);
				double f2 = static_cast<double>(dayCount(startOfY2, end)) / (Date::isLeapYear(y2) ? 366.0 : 365.0);
				return f1 + (y2 - y1 - 1) + f2;
			}
		}

		double thirty360(const Date& start, const Date& end) const {
			int d1 = std::min(start.day(), 30);
			int d2 = std::min(end.day(), 30);
			int m1 = start.month(), m2 = end.month();
			int y1 = start.year(), y2 = end.year();
			int days = 360 * (y2 - y1) + 30 * (m2 - m1) + (d2 - d1);
			return static_cast<double>(days) / 360.0;
		}

		double actual365(const Date& start, const Date& end) const {
			return static_cast<double>(dayCount(start, end)) / 365.0;
		}
	};

}