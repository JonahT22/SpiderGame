#include <iostream>

#include "LinkObjective.h"
#include "OptimizerNM.h"


OptimizerNM::OptimizerNM(const int num_links) :
	tol(2e-1),
	iterMax(5 * num_links),
	iter(0)
{
	
}

void OptimizerNM::optimize(const LinkObjective& objective, Eigen::VectorXd& x) {
	int n = x.rows();
	Eigen::VectorXd g(n);
	Eigen::MatrixXd H(n, n);
	iter = 0;
	for (size_t i = 1; i < iterMax; ++i) {
		// Evaluate f, g, and H
		double f = objective.evalObjective(x, &g, &H);
		Eigen::VectorXd dx = -1.0 * H.inverse() * g;
		x += dx;

		if (dx.norm() < tol) {
			iter = i;
			break;
		}
	}
}
