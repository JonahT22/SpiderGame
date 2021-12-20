#pragma once

#include "Optimizer.h"
#include "LinkObjective.h"

class OptimizerGDLS : public Optimizer {
public:
	OptimizerGDLS(const int num_links);
	~OptimizerGDLS() = default;
	virtual void optimize(const LinkObjective& objective, Eigen::VectorXd& x);
	
	void setAlphaInit(double alphaInit) { this->alphaInit = alphaInit; }
	void setGamma(double gamma) { this->gamma = gamma; }
	void setTol(double tol) { this->tol = tol; }
	void setIterMax(int iterMax) { this->iterMax = iterMax; }
	int getIter() const { return iter; }
	
private:
	double alphaInit;
	double gamma;
	double tol;
	int iterMax;
	int iter;
};
