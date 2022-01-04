#pragma once
#include <memory>
#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "../Utils/Transform.h"
class GameEngine;
class ShaderProgram;

/// 
/// Base class for any object that can be placed in the world and rendered on the screen
///
class SceneObject : public std::enable_shared_from_this<SceneObject> {
public:
	SceneObject(std::weak_ptr<GameEngine> engine, const std::string& name);
	~SceneObject() = default;

	// Runs after all objects are loaded, before the first frame is drawn
	virtual void BeginPlay();
	// Find this object's (world) model mtx and iterate recursively to update child matrices
	virtual void PhysicsUpdate(const float delta_time);
	// Draw this object, and ONLY this object. Do not draw children
	virtual void Render(const std::shared_ptr<ShaderProgram> shader) const;

	/* ----- Getters ----- */
	const glm::mat4& GetWorldTransformMtx() const;
	const glm::vec3& GetRelativeLocation() const;
	const glm::quat& GetRelativeRotation() const;
	const glm::vec3& GetRelativeScale() const;
	const std::string& GetName() const;
	const std::weak_ptr<SceneObject>& GetParent() const;
	// Search for a child with a given name from among the DIRECT children of this object
	std::weak_ptr<SceneObject> GetChildByName(const std::string& name);

	/* ----- Setters ----- */
	void AddChildObject(std::weak_ptr<SceneObject> new_object);
	void SetParent(std::weak_ptr<SceneObject> new_parent);
	void SetRelativeLocation(const glm::vec3 loc);
	void SetRelativeRotation(const glm::quat rot);
	void SetRelativeRotationDegrees(const glm::vec3 euler_rot);
	void SetRelativeScale(const glm::vec3 scale);
	void SetRelativeTransform(const Transform& transform);
	void MarkPhysicsDirty();

protected:
	// Does this object need to update its model matrix on the next physics tick?
	bool physicsDirty = true;
	// Name of this object, for debugging and parenting on construction
	const std::string objectName = "unnamed_object";
	// Reference to the gameengine that created this object
	std::weak_ptr<GameEngine> engineRef;
	// Transform relative to this object's parent
	Transform rootTransform;
	// Model matrix for this object (i.e. object-to-WORLD) transformation
	glm::mat4 modelMtx = glm::mat4(1.0f);
	// List of other SceneObjects that are attached to this object
	std::vector<std::weak_ptr<SceneObject> > childObjects;
	// Object that this object is attached to
	std::weak_ptr<SceneObject> parent;
};
