#pragma once

#include <memory>

#include <eigen-3.4.0/Eigen/Dense>

#include "../Rendering/SceneObject.h"
class Mesh;

///
/// Link in an IK chain. MUST be parented under an IKChain. Links location is constrained in
/// the y and z axes - they can only move on the x axis. Rotation is constrained in the x
/// and y axes - they can only rotate about the z axis.
/// TODO check on that
/// 
class Link : public SceneObject {
public:
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
	
	Link(std::weak_ptr<GameEngine> engine, const std::string& name, const float length);
	~Link() = default;

	// Inherited from SceneObject
	virtual void BeginPlay();
	virtual void PhysicsUpdate(const float delta_time);
	virtual void Render(const std::shared_ptr<ShaderProgram> shader) const;

	// Getters for the transformation matrix & its derivatives
	const Eigen::Matrix3d& GetJ() const { return J; }
	const Eigen::Matrix3d& GetJPrime() const { return JPrime; }
	const Eigen::Matrix3d& GetJ2Prime() const { return J2Prime; }

	// Set the angle (in radians) for this link
	void SetLinkAngle(double a);
	
private:
	std::shared_ptr<Mesh> linkMesh;

	// Length of this link (distance to the next link's root along the x axis)
	const float linkLength = 1.0f;

	// Store the joint transformation matrices for this link in LOCAL space,
	//   which are equal to TR(theta), TR'(theta), and TR"(theta).
	//  (Note: Scale transforms don't apply to links for this reason)
	// TODO: check that ^
	Eigen::Matrix3d J;
	Eigen::Matrix3d JPrime;
	Eigen::Matrix3d J2Prime;
};

