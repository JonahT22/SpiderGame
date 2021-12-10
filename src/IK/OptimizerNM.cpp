#include "OptimizerNM.h"
#include "LinkObjective.h"
#include <iostream>

using namespace std;
using namespace Eigen;

OptimizerNM::OptimizerNM(const int num_links) :
	tol(1e-1),
	iterMax(5 * num_links),
	iter(0)
{
	
}

void OptimizerNM::optimize(const LinkObjective& objective, VectorXd& x) {
	int n = x.rows();
	VectorXd g(n);
	MatrixXd H(n, n);
	iter = 0;
	for (size_t i = 1; i < iterMax; ++i) {
		// Evaluate f, g, and H
		double f = objective.evalObjective(x, &g, &H);
		VectorXd dx = -1.0 * H.inverse() * g;
		x += dx;

		if (dx.norm() < tol) {
			iter = i;
			break;
		}
	}
}
