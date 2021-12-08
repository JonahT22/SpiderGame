#pragma once
#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include <memory>
#include <Eigen/Dense>

class LinkObjective;

class Optimizer
{
public:
	Optimizer() {};
	virtual ~Optimizer() {};
	virtual void optimize(const std::shared_ptr<LinkObjective> objective, Eigen::VectorXd& x) = 0;
};

#endif
