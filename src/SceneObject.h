#pragma once
#include <vector>
#include <memory>

#include <glm/glm.hpp>

#include "Transform.h"
class ShaderProgram;

/// 
/// Base class for any object that can be placed in the world and rendered on the screen
///
class SceneObject {
public:
	SceneObject() = default;
	// TODO: when this object dies, all its children should probably be destroyed too
	~SceneObject() = default;

	/* ----- Other Functions ----- */
	void AddChildObject(std::shared_ptr<SceneObject> new_object);
	// Given the local->world transform of this object's parent, find this object's model mtx
	virtual void PhysicsUpdate(const glm::mat4& parent_transform);
	virtual void Render(const std::shared_ptr<ShaderProgram> shader) = 0;

protected:
	// Transform relative to this object's parent
	Transform rootTransform;
	// Model matrix for this object (i.e. object-to-WORLD) transformation
	glm::mat4 modelMtx;
	// List of SceneObjects that are attached to this object
	std::vector<std::weak_ptr<SceneObject> > childObjects;
};
