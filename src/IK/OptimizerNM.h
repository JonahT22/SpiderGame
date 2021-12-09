#pragma once
#ifndef OPTIMIZER_NM_H
#define OPTIMIZER_NM_H

#include "Optimizer.h"
#include "LinkObjective.h"

class OptimizerNM : public Optimizer
{
public:
	OptimizerNM(const int num_links);
	~OptimizerNM() = default;
	virtual void optimize(const LinkObjective& objective, Eigen::VectorXd& x);
	
	void setTol(double tol) { this->tol = tol; }
	void setIterMax(int iterMax) { this->iterMax = iterMax; }
	int getIter() const { return iter; }
	
private:
	double tol;
	int iterMax;
	int iter;
};

#endif
