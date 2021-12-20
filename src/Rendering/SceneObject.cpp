#include <iostream>

#include "../Player/Camera.h"
#include "../GameEngine.h"
#include "SceneObject.h"
#include "ShaderProgram.h"

SceneObject::SceneObject(std::weak_ptr<GameEngine> engine, const std::string& name) :
	engineRef(engine),
	objectName(name)
{}

void SceneObject::BeginPlay() {}

void SceneObject::PhysicsUpdate(const float delta_time) {
	// Only recalculate model matrix if this object's physics are dirty
	if (physicsDirty) {
		// Find this object's local -> world transform using the parent's transform
		if (!parent.expired()) {
			modelMtx = parent.lock()->GetWorldTransformMtx() * rootTransform.GetMatrix();
		}
		else {
			// If this object has no parent, then its relative transform is the same as its
			//   world transform
			modelMtx = rootTransform.GetMatrix();
		}
		// Now that the physics have been updated, this object is no longer dirty
		physicsDirty = false;
	}

	// Update the physics of children - this lets any new physics changes to parents be
	//   immediately propagated to children
	for (auto& child : childObjects) {
		if (!child.expired()) {
			child.lock()->PhysicsUpdate(delta_time);
		}
		else {
			std::cerr << "ERROR: Attempted to update physics on an invalid ";
			std::cerr << "child object!" << std::endl;
		}
	}
}

void SceneObject::Render(const std::shared_ptr<ShaderProgram> shader) const {
	// The shader should already be bound before drawing this mesh
	if (!shader->IsShaderActive()) {
		std::cout << "WARNING: Shader object was not bound before rendering a SceneObject.";
		std::cout << " For best performance, shaders should not be activated/deactivated";
		std::cout << " on a per-object basis. Activating the shader for this object...";
		std::cout << std::endl;
		shader->Activate();
	}

	// Try getting a shared reference to the game engine
	if (std::shared_ptr<GameEngine> engine = engineRef.lock()) {
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

const glm::mat4& SceneObject::GetWorldTransformMtx() const {
	return modelMtx;
}

const glm::vec3& SceneObject::GetRelativeLocation() const {
	return rootTransform.loc;
}

const glm::quat& SceneObject::GetRelativeRotation() const {
	return rootTransform.rot;
}

const glm::vec3& SceneObject::GetRelativeScale() const {
	return rootTransform.scale;
}

const std::string& SceneObject::GetName() const {
	return objectName;
}

const std::weak_ptr<SceneObject>& SceneObject::GetParent() const {
	return parent;
}

std::weak_ptr<SceneObject> SceneObject::GetChildByName(const std::string& name) {
	for (auto& child : childObjects) {
		if (child.lock()->GetName() == name) {
			return child;
		}
	}
	std::cerr << "WARNING: No child with name " << name << " found on parent object ";
	std::cerr << objectName << "!" << std::endl;
	return std::weak_ptr<SceneObject>();
}

void SceneObject::AddChildObject(std::weak_ptr<SceneObject> new_object) {
	childObjects.emplace_back(new_object);
	// Pass a weak reference to the new child
	new_object.lock()->SetParent(enable_shared_from_this::weak_from_this());
	if (enable_shared_from_this::weak_from_this().expired()) {
		std::cerr << "WARNING: Passing expired weak_from_this pointer from parent object ";
		std::cerr << objectName << " to child " << new_object.lock()->GetName();
		std::cerr << ". Check that you aren't trying to call weak_from_this ";
		std::cerr << "from an object's constructor!" << std::endl;
	}
	MarkPhysicsDirty();
}

void SceneObject::SetParent(std::weak_ptr<SceneObject> new_parent) {
	parent = new_parent;
	MarkPhysicsDirty();
}

void SceneObject::SetRelativeLocation(const glm::vec3 loc) {
	rootTransform.loc = loc;
	MarkPhysicsDirty();
}

void SceneObject::SetRelativeRotation(const glm::quat rot) {
	rootTransform.rot = rot;
	MarkPhysicsDirty();
}

void SceneObject::SetRelativeRotationDegrees(const glm::vec3 euler_rot) {
	rootTransform.rot = Transform::EulerToQuat(glm::radians(euler_rot));
	MarkPhysicsDirty();
}

void SceneObject::SetRelativeScale(const glm::vec3 scale) {
	rootTransform.scale = scale;
	MarkPhysicsDirty();
}

void SceneObject::SetRelativeTransform(const Transform& transform) {
	rootTransform = transform;
	MarkPhysicsDirty();
}

void SceneObject::MarkPhysicsDirty() {
	// If this object is already marked as dirty, there's no need to re-mark it
	if (!physicsDirty) {
		physicsDirty = true;
		// If this object is dirty, all of its children are dirty too
		for (auto& child : childObjects) {
			if (!child.expired()) {
				child.lock()->MarkPhysicsDirty();
			}
			else {
				std::cerr << "ERROR: Attempted to mark physics dirty on an invalid ";
				std::cerr << "child object!" << std::endl;
			}
		}
	}
}
