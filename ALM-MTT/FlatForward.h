#pragma once

#include <cmath>
#include "Date.h"
#include "DayCounter.h"
#include "YieldCurve.h"

namespace ALM {

	class FlatForward : public YieldCurve {
	public:
		FlatForward(const Date& ref, double rate, DayCounter dc) :
			ref_(ref), rate_(rate), dc_(dc) { }

		double discount(const Date& t) const override {
			double yf = dc_.yearFraction(ref_, t);
			return std::pow(1 + rate_, -yf);
		}
		virtual double zero(const Date& t) const override {
			return rate_;
		}
		virtual double forward(const Date& t1, const Date& t2) const override {
			return rate_;
		}
		virtual Date reference() const {
			return ref_;
		}
	private:
		Date ref_;
		double rate_;
		DayCounter dc_;
	};

}