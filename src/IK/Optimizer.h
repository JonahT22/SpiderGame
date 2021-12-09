#pragma once

#include <memory>
#include <eigen-3.4.0/Eigen/Dense>

#include "LinkObjective.h"

class Optimizer {
public:
	Optimizer() {};
	virtual ~Optimizer() {};
	virtual void optimize(const LinkObjective& objective, Eigen::VectorXd& x) = 0;
};
