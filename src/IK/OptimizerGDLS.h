#pragma once
#ifndef OPTIMIZER_GDLS_H
#define OPTIMIZER_GDLS_H

#include "Optimizer.h"

class LinkObjective;

class OptimizerGDLS : public Optimizer
{
public:
	OptimizerGDLS(const int num_links);
	virtual ~OptimizerGDLS();
	virtual void optimize(const std::shared_ptr<LinkObjective> objective, Eigen::VectorXd& x);
	
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

#endif
