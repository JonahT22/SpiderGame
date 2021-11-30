#include "SceneObject.h"

#include <iostream>
void SceneObject::AddChildObject(std::shared_ptr<SceneObject> new_object) {
	// TODO: should probably check that the new object doesn't have this object
	//   in any of its children. Can do this by giving each object a unique ID and 
	//   searching for this ID among the new_object's children
	childObjects.emplace_back(std::weak_ptr<SceneObject>(new_object));
}

void SceneObject::PhysicsUpdate(const glm::mat4& parent_transform) {
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
