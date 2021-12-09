#include "IKChain.h"

#include <iostream>
#include <string>
#include <vector>
#include <cassert>

#include "Link.h"

IKChain::IKChain(std::weak_ptr<GameEngine> engine, const std::string& name) :
	SceneObject(engine, name) {
	J_endEffectorPos << 1.0, 0.0, 1.0;
}

void IKChain::WrapAngles(Eigen::VectorXd& angles)
{
	// Wrap each angle from -pi to pi
	size_t num_angles = angles.rows();
	double pi = 3.1415926535;
	for (size_t i = 0; i < num_angles; ++i) {
		while (angles(i) > pi) {
			angles(i) -= 2.0 * pi;
		}
		while (angles(i) < -1.0 * pi) {
			angles(i) += 2.0 * pi;
		}
	}
}

void IKChain::BeginPlay() {
	// Create the links in the chain
	// Note: this must be done in beginplay, since weak_from_this is called in AddChildObject
	for (size_t i = 0; i < numLinks; ++i) {
		std::string link_name = objectName + "_link_" + std::to_string(i);
		auto new_link = std::make_shared<Link>(engineRef, link_name);
		new_link->SetRelativeLocation(glm::vec3(0.5f, 0.0f, 0.0f));
		// TODO: set link properties
		if (allLinks.size() == 0) {
			// Parent the first link to this object
			AddChildObject(new_link);
		}
		else {
			// Parent all other links to the most previously-added link
			allLinks.back()->AddChildObject(new_link);
		}
		allLinks.emplace_back(new_link);
	}

	J_linkAngles = Eigen::VectorXd::Zero(numLinks);
	// TODO: set J-space position to (was [1, 0] but should probably be the same as the
	//   loc from the rootTransform)
	UpdateLinkAngles();
	
	// Manually call beginplay on children, since they aren't managed by the scene
	for (auto& link : allLinks) {
		link->BeginPlay();
	}
}

void IKChain::PhysicsUpdate() {
	// TODO: this is where I'll call the optimizer stuff

	SceneObject::PhysicsUpdate();
}

void IKChain::Render(const std::shared_ptr<ShaderProgram> shader) const {
	// Manually render all child links, since the scene doesn't manage them
	for (auto& link : allLinks) {
		link->Render(shader);
	}
}

Eigen::Vector3d IKChain::GetEndEffectorPos() const {
	return J_endEffectorPos;
}

size_t IKChain::GetNumLinks() const {
	return allLinks.size();;
}

const Eigen::Matrix3d& IKChain::GetJMatrix(size_t link_idx, size_t derivative) const
{
	assert(derivative <= 2);
	if (derivative == 0) {
		return allLinks.at(link_idx)->GetJ();
	}
	else if (derivative == 1) {
		return allLinks.at(link_idx)->GetJPrime();
	}
	else if (derivative == 2) {
		return allLinks.at(link_idx)->GetJ2Prime();
	}
	else {
		std::cerr << "Error: Accessing invalid link matrix derivative!" << std::endl;
		return Eigen::Matrix3d::Identity();
	}
}

Eigen::VectorXd IKChain::GetLinkAngles() const {
	return J_linkAngles;
}

void IKChain::SetEndEffector(double x, double y){
	J_endEffectorPos << x, y, 1.0;
}

void IKChain::SetLinkAngles(const Eigen::VectorXd& new_angles)
{
	assert(new_angles.rows() == J_linkAngles.rows());
	// Copy the new link angles into this class' angle list (for future reference)
	J_linkAngles = new_angles;
	UpdateLinkAngles();
}

// PRIVATE FUNCTIONS

void IKChain::UpdateLinkAngles() {
	for (size_t i = 0; i < allLinks.size(); ++i) {
		allLinks.at(i)->SetAngle(J_linkAngles(i));
	}
}