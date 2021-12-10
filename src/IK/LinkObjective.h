#pragma once

#include <memory>

#include <eigen-3.4.0/Eigen/Dense>

#include "Link.h"

class IKChain;

class LinkObjective {
public:
	LinkObjective();
	~LinkObjective() = default;
	// Return the objective function f, with optional args for gradient and Hessian
	double evalObjective(const Eigen::VectorXd& theta, Eigen::VectorXd* g = nullptr, Eigen::MatrixXd* H = nullptr) const;

	void SetChainRef(std::shared_ptr<IKChain> _chain) { chain = _chain; }
	void SetTarget(const Eigen::Vector2d& p) { pTarget = p; }

private:
	// Custom constraint function. 0 if constraints are satisfied, 1 if they are not
	double CalcConstraintFactor(const Eigen::VectorXd& theta) const;

	// Keep a reference to the chain that this objective function is evaluating
	std::shared_ptr<IKChain> chain;
	// The most recent target position to compare the chain with
	Eigen::Vector2d pTarget;
	// Target weight - prioritizes getting to the target location
	double wTar;
	// Regulizer weight - prioritizes minimizing the angles
	double wReg;
	// Constraint weight - prioritizes keeping the angle constraints
	double wCon;
};

