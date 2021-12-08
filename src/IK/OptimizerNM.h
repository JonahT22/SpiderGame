#pragma once
#ifndef OPTIMIZER_NM_H
#define OPTIMIZER_NM_H

#include "Optimizer.h"

class LinkObjective;

class OptimizerNM : public Optimizer
{
public:
	OptimizerNM(const int num_links);
	virtual ~OptimizerNM();
	virtual void optimize(const std::shared_ptr<LinkObjective> objective, Eigen::VectorXd& x);
	
	void setTol(double tol) { this->tol = tol; }
	void setIterMax(int iterMax) { this->iterMax = iterMax; }
	int getIter() const { return iter; }
	
private:
	double tol;
	int iterMax;
	int iter;
};

#endif
