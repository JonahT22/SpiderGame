#include "LinkObjective.h"
#include <cmath>
#include <iostream>
#include <cassert>

#include "IKChain.h"

LinkObjective::LinkObjective() :
	wTar(1e3),
	wReg(1e1),
	wCon(1e2),
	pTarget(0.0, 0.0) {}

double LinkObjective::evalObjective(const Eigen::VectorXd& theta,
                                    Eigen::VectorXd* g,
                                    Eigen::MatrixXd* H) const {
	assert(chain);
	// Update the chain with the new angles
	chain->SetLinkAngles(theta);
	const size_t num_links = chain->GetNumLinks();
	const Eigen::Vector3d r = chain->GetEndEffectorPos();
	// Use a temporary var to accumulate the transformation matrices of each link
	Eigen::Vector3d pTemp = r;

	// Find the position vector (location of end effector in world space)
	for (int link_idx = num_links - 1; link_idx >= 0; --link_idx) {
		pTemp = chain->GetJMatrix(link_idx, 0) * pTemp;
	}
	Eigen::Vector2d p = pTemp.head<2>();

	Eigen::Vector2d dp = p - pTarget;

	// Assuming the starting angles of all links are 0
	Eigen::VectorXd dTheta = theta;
	
	// Calculate objective value. Note: .value() is required to convert a 1x1 matrix to a double
	double target_factor = wTar * dp.transpose() * dp;
	double reg_factor = (wReg * dTheta.transpose() * dTheta).value();
	double constraint_factor = wCon * CalcConstraintFactor(dTheta);
	double f = (target_factor + constraint_factor + reg_factor) / 3.0;

	// If g and/or H are provided, calculate the derivatives of the position vector w.r.t each theta, then
	// set the gradient vector and Hessian matrix
	if (g != nullptr) {
		Eigen::MatrixXd PPrime(2, num_links);
		// Find the derivative of the P vector w.r.t theta
		for (size_t p_idx = 0; p_idx < num_links; ++p_idx) {
			pTemp = r;
			for (int link_idx = num_links - 1; link_idx >= 0; --link_idx) {
				// If the current link matches the p vector that is being set, get J' instead of J
				size_t derivative = (size_t)(link_idx == p_idx);
				const auto& J = chain->GetJMatrix(link_idx, derivative);
				pTemp = chain->GetJMatrix(link_idx, derivative) * pTemp;
			}
			// Set the block of values at position 0, pidx and size 2, 1 to the p vector
			PPrime.block(0, p_idx, 2, 1) << pTemp.head<2>();
		}

		// Find the gradient, which requires the dot product of dp with each p vector in PPrime
		Eigen::VectorXd dpDotPPrime(num_links);
		for (size_t p_idx = 0; p_idx < num_links; ++p_idx) {
			dpDotPPrime(p_idx) = (dp.transpose() * PPrime.block(0, p_idx, 2, 1)).value();
		}
		*g = wTar * dpDotPPrime + wReg * dTheta;

		// The hessian can only be provided if g is also provided
		if (H != nullptr) {
			// Find the 2nd derivative of the position vector w.r.t theta
			Eigen::MatrixXd P2Prime(2 * num_links, num_links);
			for (size_t row = 0; row < num_links; ++row) {
				for (size_t col = 0; col < num_links; ++col) {
					pTemp = r;
					for (int link_idx = num_links - 1; link_idx >= 0; --link_idx) {
						size_t derivative = (size_t)(link_idx == row) + (size_t)(link_idx == col);
						pTemp = chain->GetJMatrix(link_idx, derivative) * pTemp;
					}
					P2Prime.block(2 * row, col, 2, 1) << pTemp.head<2>();
				}
			}

			// Find the Hessian, which requires the dot of dp with each p vector in P2Prime
			Eigen::MatrixXd dpDotP2Prime(num_links, num_links);
			for (size_t row = 0; row < num_links; ++row) {
				for (size_t col = 0; col < num_links; ++col) {
					dpDotP2Prime(row, col) = (dp.transpose() * P2Prime.block(2 * row, col, 2, 1)).value();
				}
			}
			*H = wTar * (PPrime.transpose() * PPrime + dpDotP2Prime) + wReg * Eigen::MatrixXd::Identity(num_links, num_links);
		}
	}

	return f;
}

double LinkObjective::CalcConstraintFactor(const Eigen::VectorXd& theta) const {
	
	double sum = 0.0;
	// Array of ideal resting angles for each leg
	constexpr double pi = 3.14159;
	// in degrees: 45, -105, 0, ...
	double rest_angles[8] = { pi / 4.0, (-7.0 * pi) / 12.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
	assert(theta.size() < 8);
	// Take the squared sum of the difference between each angle and its goal
	for (size_t i = 0; i < theta.size(); ++i) {
		double dt = theta(i) - rest_angles[i];
		sum += dt * dt;
	}
	return sum;
}
