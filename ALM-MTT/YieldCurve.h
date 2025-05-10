#pragma once

#include "Date.h"

namespace ALM {

	class YieldCurve {
	public:
		virtual ~YieldCurve() = default;
		virtual double discount(const Date& t) const = 0;
		virtual double zero(const Date& t) const = 0;
		virtual double forward(const Date& t1, const Date& t2) const = 0;
		virtual Date reference() const = 0;
	};

}