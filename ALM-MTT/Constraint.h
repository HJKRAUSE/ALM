#pragma once

#include <Eigen/dense>

namespace ALM {

	class Constraint {
	public:
		virtual ~Constraint() = default;
		virtual void project(Eigen::VectorXd& x) const = 0;
		virtual bool isSatisfied(const Eigen::VectorXd& x) const = 0;
	protected:
		Constraint() = default;
	};

}