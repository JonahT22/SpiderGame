#pragma once
#include <vector>
#include <memory>

#include <glm/glm.hpp>

#include "Transform.h"
class GameEngine;
class ShaderProgram;

/// 
/// Base class for any object that can be placed in the world and rendered on the screen
///
class SceneObject {
public:
	SceneObject(std::weak_ptr<const GameEngine> engine, const std::string& name);
	~SceneObject() = default;

	/* ----- Getters ----- */
	glm::vec3 GetRelativeLocation() const;
	glm::quat GetRelativeRotation() const;
	glm::vec3 GetRelativeScale() const;
	// TODO: GetParent

	/* ----- Setters ----- */
	void SetRelativeLocation(glm::vec3 loc);
	void SetRelativeRotation(glm::quat rot);
	void SetRelativeRotationDegrees(glm::vec3 euler_rot);
	void SetRelativeScale(glm::vec3 scale);

	/* ----- Other Functions ----- */
	void AddChildObject(std::shared_ptr<SceneObject> new_object);
	// Given the local->world transform of this object's parent, find this
	//   object's (world) model mtx and iterate recursively to update child matrices
	virtual void PhysicsUpdate(const glm::mat4& parent_transform);
	// Draw this object, and ONLY this object. Do not draw children
	virtual void Render(const std::shared_ptr<ShaderProgram> shader);

protected:
	// Name of this object, for debugging and parenting on construction
	const std::string objectName;
	// Reference to the gameengine that created this object
	std::weak_ptr<const GameEngine> engineRef;
	// Transform relative to this object's parent
	Transform rootTransform;
	// Model matrix for this object (i.e. object-to-WORLD) transformation
	glm::mat4 modelMtx = glm::mat4(1.0f);
	// List of other SceneObjects that are attached to this object
	std::vector<std::weak_ptr<SceneObject> > childObjects;
};
