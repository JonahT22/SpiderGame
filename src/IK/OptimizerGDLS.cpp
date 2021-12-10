#include "OptimizerGDLS.h"
#include "LinkObjective.h"
#include <iostream>

using namespace std;
using namespace Eigen;

OptimizerGDLS::OptimizerGDLS(const int num_links) :
	alphaInit(1.0),
	gamma(0.25),
	tol(2e-1),
	iterMax(5 * num_links),
	iter(0)
{
	
}

void OptimizerGDLS::optimize(const LinkObjective& objective, VectorXd& x) {
	int n = x.rows();
	VectorXd g(n);
	VectorXd dx(n);
	iter = 0;
	for (size_t i = 1; i <= iterMax; ++i) {
		// Evaluate f and g
		double f = objective.evalObjective(x, &g);
		// Perform line search
		double alpha = alphaInit;
		for (size_t j = 1; j < iterMax; ++j) {
			// Test the function at the new location, and reduce alpha if it is not smaller
			dx = -1.0 * alpha * g;
			double fNew = objective.evalObjective(x + dx);
			if (fNew < f) {
				// The current alpha value works
				break;
			}
			// If the new val is not better, reduce the step size
			alpha *= gamma; 
		}
		// Advance x using the chosen step size
		x += dx;

		if (dx.norm() < tol) {
			iter = i;
			break;
		}
	}
}
