#include "SceneObject.h"

#include <iostream>

#include "ShaderProgram.h"

glm::vec3 SceneObject::GetRelativeLocation() const {
	return rootTransform.loc;
}

glm::quat SceneObject::GetRelativeRotation() const {
	return rootTransform.rot;
}

glm::vec3 SceneObject::GetRelativeScale() const {
	return rootTransform.scale;
}

void SceneObject::SetRelativeLocation(glm::vec3 loc) {
	rootTransform.loc = loc;
}

void SceneObject::SetRelativeRotation(glm::quat rot) {
	rootTransform.rot = rot; 
}

void SceneObject::SetRelativeRotationDegrees(glm::vec3 euler_rot) {
	rootTransform.rot = Transform::EulerToQuat(glm::radians(euler_rot));
}

void SceneObject::SetRelativeScale(glm::vec3 scale) {
	rootTransform.scale = scale;
}

void SceneObject::AddChildObject(std::shared_ptr<SceneObject> new_object) {
	childObjects.emplace_back(std::weak_ptr<SceneObject>(new_object));
}

void SceneObject::PhysicsUpdate(const glm::mat4& parent_transform) {
	// TODO: don't pass in parent transform. Instead, store a weak ref to the parent,
	//   and get the parent's model mtx directly.
	// If this weak ref is invalid, use an identity matrix instead

	// Find this object's local -> world transform
	modelMtx = parent_transform * rootTransform.GetMatrix();
	modelMtxInvT = glm::transpose(glm::inverse(modelMtx));

	// Propagate this object's transformation matrix to its children
	for (auto& child : childObjects) {
		if (!child.expired()) {
			child.lock()->PhysicsUpdate(modelMtx);
		}
		else {
			std::cerr << "ERROR: Attempted to update physics on an invalid child object!";
			std::cerr << std::endl;
		}
	}
}

void SceneObject::Render(const std::shared_ptr<ShaderProgram> shader) {
	// The shader should already be bound before drawing this mesh
	if (!shader->IsShaderActive()) {
		std::cout << "WARNING: Shader object was not bound before rendering a mesh.";
		std::cout << " For best performance, shaders should not be activated/deactivated";
		std::cout << " on a per-mesh basis. Activating the shader for this mesh...";
		std::cout << std::endl;
		shader->Activate();
	}
	shader->SetMat4Uniform("M", modelMtx);
	shader->SetMat4Uniform("M_invT", modelMtxInvT);
}
