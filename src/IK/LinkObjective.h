#pragma once

#include <memory>

#include <Eigen/Dense>

#include "Link.h"

class Chain;

class LinkObjective
{
public:
	LinkObjective();
	virtual ~LinkObjective();
	// Return the objective function f, with optional args for gradient and Hessian
	double evalObjective(const Eigen::VectorXd& theta, Eigen::VectorXd* g = nullptr, Eigen::MatrixXd* H = nullptr) const;

	void SetChainRef(std::shared_ptr<Chain> _chain) { chain = _chain; }
	void SetTarget(const Eigen::Vector2d& p) { pTarget = p; }

private:
	// Keep a reference to the chain that this objective function is evaluating
	std::shared_ptr<Chain> chain;
	// The most recent target position to compare the chain with
	Eigen::Vector2d pTarget;
	// Target weight - prioritizes getting to the target location
	double wTar;
	// Regulizer weight - prioritizes minimizing the angle weights
	double wReg;
};

