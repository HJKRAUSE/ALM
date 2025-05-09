#pragma once

#include <Eigen/dense>
#include <functional>

namespace ALM {

	struct SolverResults {
		Eigen::VectorXd x;
		double objective;
		int iterations;
		bool success;
	};

	class Solver {
	public:
		virtual ~Solver() = default;
		virtual SolverResults solve(
			const std::function<double(Eigen::VectorXd)>& f, 
			const Eigen::VectorXd& x0) = 0;
	protected:
		Solver() = default;
	};


}