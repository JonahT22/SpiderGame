#include "IKChain.h"

#include <iostream>
#include <string>
#include <vector>
#include <cassert>

#include <eigen-3.4.0/Eigen/Dense>
// TODO
#include <glm/gtx/string_cast.hpp>

#include "Link.h"
#include "OptimizerGDLS.h"
#include "OptimizerNM.h"
#include "../SceneObject.h"

IKChain::IKChain(std::weak_ptr<GameEngine> engine, const std::string& name) :
	SceneObject(engine, name), optimizerGDLS(numLinks), optimizerNM(numLinks) {
	// TODO: endEffectorPos should == the linkLength of the final link in the chain: [len, 0]
	//   Right now, this is hardcoded to 0.5, so keep the same hardcoding here
	J_endEffectorPos << 0.5, 0.0, 1.0;
}

// TODO: move this
void IKChain::WrapAngles(Eigen::VectorXd& angles) {
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
	// Find this chain's target point. The target should be attached to the spider
	//   character, which is the Chain's parent object
	target = parent.lock()->GetChildByName(objectName + "_target");
	if (target.expired()) {
		std::cerr << "ERROR Getting target ref in IKChain!" << std::endl;
	}

	// Create the link root
	linkRoot = std::make_shared<SceneObject>(engineRef, objectName + "link_root");
	AddChildObject(linkRoot);

	// Get a shared ptr to self (SceneObject), then cast to IKChain for objectiveFunction
	objectiveFunc.SetChainRef(std::dynamic_pointer_cast<IKChain>(shared_from_this()));
	
	// Create the links in the chain
	// TODO: remove hardcoding
	// The first link in the chain should be shorter than the others
	const float link_offsets[8] = { 0.0f, 0.4, 0.6, 0.6, 0.6, 0.6, 0.6, 0.6 };
	constexpr float pi = glm::pi<float>();
	if (numLinks > 7) {
		std::cerr << "ERROR: didn't hardcode that many link lengths!" << std::endl;
		abort();
	}

	// Note: this must be done in beginplay, since weak_from_this is called in AddChildObject
	for (size_t i = 0; i < numLinks; ++i) {
		std::string link_name = objectName + "_link_" + std::to_string(i);
		
		// Set the link lengths and locations
		float link_offset = link_offsets[i];
		float link_len = link_offsets[i + 1];
		auto new_link = std::make_shared<Link>(engineRef, link_name, link_len);
		new_link->SetRelativeLocation(glm::vec3(link_offset, 0.0f, 0.0f));

		if (allLinks.size() == 0) {
			// Parent the first link to the link root
			linkRoot->AddChildObject(new_link);
		}
		else {
			// Parent all other links to the most previously-added link
			allLinks.back()->AddChildObject(new_link);
		}
		allLinks.emplace_back(new_link);
	}

	// Initialize the angles with some hardcoded values. Used as a starting point for optimizer
	J_linkAngles = Eigen::VectorXd::Zero(numLinks);
	double start_angles[8] = { pi / 4.0, (-7.0 * pi) / 12.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
	for (size_t i = 0; i < numLinks; ++i) {
		J_linkAngles(i) = start_angles[i];
	}
	UpdateLinkAngles();
	
	// Manually call beginplay on children, since they aren't managed by the scene
	for (auto& link : allLinks) {
		link->BeginPlay();
	}
}

void IKChain::PhysicsUpdate(const float delta_time) {
	// Get the world-space position of the target
	glm::vec4 target_loc = target.lock()->GetWorldTransformMtx()[3];
	// Get the local-space position of the target
	target_loc = glm::inverse(modelMtx) * target_loc;
	// Rotate the Chain to face the target location
	// TODO: Figure out why this -1.0f is necessary
	float rot_angle = -1.0f * atan2(target_loc.z, target_loc.x);
	if (abs(rot_angle) < glm::radians(70.0)) {
		linkRoot->SetRelativeRotation(glm::vec3(0.0f, rot_angle, 0.0f));
	}

	// Set up the local-space target for the objective evaluation
	Eigen::Vector2d x(target_loc.x, target_loc.y);
	objectiveFunc.SetTarget(x);

	// Perform GLDS optimization (note: chain angles are updated in the optimizer)
	Eigen::VectorXd anglesGLDS = GetLinkAngles();
	optimizerGDLS.optimize(objectiveFunc, anglesGLDS);

	// Perform newton's method from where GLDS left off
	Eigen::VectorXd anglesNM = anglesGLDS;
	optimizerNM.optimize(objectiveFunc, anglesNM);

	// Check if newton's method improved the result
	Eigen::VectorXd angles = (objectiveFunc.evalObjective(anglesNM) < objectiveFunc.evalObjective(anglesGLDS)) ?
		anglesNM : anglesGLDS;

	// Wrap the angles and update the chain one final time
	// TODO: just put the function here
	WrapAngles(angles);
	SetLinkAngles(angles);

	SceneObject::PhysicsUpdate(delta_time);
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

const Eigen::Matrix3d& IKChain::GetJMatrix(size_t link_idx, size_t derivative) const {
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

void IKChain::SetLinkAngles(const Eigen::VectorXd& new_angles) {
	assert(new_angles.rows() == J_linkAngles.rows());
	// Copy the new link angles into this class' angle list (for future reference)
	J_linkAngles = new_angles;
	UpdateLinkAngles();
}

// PRIVATE FUNCTIONS

void IKChain::UpdateLinkAngles() {
	for (size_t i = 0; i < allLinks.size(); ++i) {
		allLinks.at(i)->SetLinkAngle(J_linkAngles(i));
	}
}