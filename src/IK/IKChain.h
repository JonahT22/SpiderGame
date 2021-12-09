#pragma once
#include <vector>
#include <memory>
#include <string>

#include <eigen-3.4.0/Eigen/Dense>

#include "../SceneObject.h"
class Link;
class GameEngine;
class ShaderProgram;
class LegTarget;

/// 
/// TODO
/// Note: A prefix of J_ means the quantity is in relation to the local 'J' space of
/// the chain, which is a 2D space for chain optimization
/// 
class IKChain : public SceneObject {
public:
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW;
	// Wrap angles to the range [-pi, pi]
	static void WrapAngles(Eigen::VectorXd& angles);

	IKChain(std::weak_ptr<GameEngine> engine, const std::string& name);
	~IKChain() = default;

	// Inherited from SceneObject
	virtual void BeginPlay();
	virtual void PhysicsUpdate();
	virtual void Render(const std::shared_ptr<ShaderProgram> shader) const;

	/* ----- Getters ----- */
	Eigen::Vector3d GetEndEffectorPos() const;
	size_t GetNumLinks() const;
	const Eigen::Matrix3d& GetJMatrix(size_t link_idx, size_t derivative) const;
	Eigen::VectorXd GetLinkAngles() const;

	/* ----- Setters ----- */
	void SetEndEffector(double x, double y);
	void SetLinkAngles(const Eigen::VectorXd& new_angles);

private:
	// Iterates over each link and recalculates its J-space transform matrices with new angles
	void UpdateLinkAngles();

	// TODO: set this from file
	size_t numLinks = 3;

	// IKChain has exclusive control over the links attached to it, so make this a vector
	//   of shared_ptrs instead of weak_ptrs. IKChain manages their lifetime, not the Scene
	std::vector<std::shared_ptr<Link> > allLinks;

	// Actor that stores the target point for the IK chain
	std::shared_ptr<LegTarget> legTarget;

	// Angle of each link in the chain, used to quickly get the chain's current state as
	//   a starting point for optimization
	Eigen::MatrixXd J_linkAngles;
	// Location of the end-effector 'r', in the local space of the final link
	// Note: 2D coordinate, with w = 1.0
	Eigen::Vector3d J_endEffectorPos;
};

