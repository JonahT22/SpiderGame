#pragma once

#include <glm/glm.hpp>

#include "Transform.h"

/// 
/// Base class for any object that can be placed in the world and rendered on the screen
///
class SceneObject {
public:
	SceneObject() = default;
	~SceneObject() = default;

	virtual void Render() = 0;
	virtual void PhysicsUpdate() = 0;
private:
	// Transform relative to this object's parent
	Transform rootTransform;
	// Model matrix for this object (i.e. object-to-WORLD) transformation
	glm::mat4 modelMtx;
};
