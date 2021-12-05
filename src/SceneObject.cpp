#include "SceneObject.h"

#include <iostream>

#include "ShaderProgram.h"
#include "GameEngine.h"
#include "Camera.h"

SceneObject::SceneObject(std::weak_ptr<const GameEngine> engine, const std::string& name) :
	engineRef(engine),
	objectName(name)
{}

glm::vec3 SceneObject::GetRelativeLocation() const {
	return rootTransform.loc;
}

glm::quat SceneObject::GetRelativeRotation() const {
	return rootTransform.rot;
}

glm::vec3 SceneObject::GetRelativeScale() const {
	return rootTransform.scale;
}

std::string SceneObject::GetName() const {
	return objectName;
}

void SceneObject::SetRelativeLocation(const glm::vec3 loc) {
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

	// Try getting a shared reference to the game engine
	if (std::shared_ptr<const GameEngine> engine = engineRef.lock()) {
		// Get the main camera to access the view and projection matrices
		std::shared_ptr<Camera> main_camera = engine->GetMainCamera();
		const glm::mat4 model_view_mtx = main_camera->GetViewMtx() * modelMtx;
		// Shaders might use the Mv matrix, or the Mvp matrix. Try sending both, and the
		//   shader will ignore whichever is not being used.
		// Note: Shaders should never use BOTH Mv and Mvp
		if (shader->GetUniform("Mv") != -1) {
			shader->SetMat4Uniform("Mv", model_view_mtx, true);
		}
		else if (shader->GetUniform("Mvp") != -1) {
			shader->SetMat4Uniform("Mvp", main_camera->GetProjectionMtx() *
			                       model_view_mtx, true);
		}
		// If the shader needs to transform normals into world space, it also needs the
		//   inverse transpose of the modelview matrix:
		if (shader->GetUniform("Mv_invT") != -1) {
			shader->SetMat4Uniform("Mv_invT",
			                       glm::transpose(glm::inverse(model_view_mtx)), true);
		}
	}
	else {
		std::cerr << "ERROR: Invalid engineRef access in SceneObject!" << std::endl;
	}
}
