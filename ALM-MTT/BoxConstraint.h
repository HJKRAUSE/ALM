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

#include <Eigen/dense>
#include "Constraint.h"

namespace ALM {

	class BoxConstraint : public Constraint {
	public:
		BoxConstraint(const Eigen::VectorXd& lower, const Eigen::VectorXd& upper) :
			lower_(lower), upper_(upper) { }

		void project(Eigen::VectorXd& x) const override {
			for (int i = 0; i < x.size(); i++) {
				x[i] = std::clamp(x[i], lower_[i], upper_[i]);
			}
		}
		bool isSatisfied(const Eigen::VectorXd& x) const override {
			return ((x.array() >= lower_.array()) && (x.array() <= upper_.array())).all();
		}
	private:
		Eigen::VectorXd upper_;
		Eigen::VectorXd lower_;
	};

}