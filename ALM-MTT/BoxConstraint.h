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