#pragma once

#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "SceneObject.h"

/// 
/// Controllable camera that always points toward its root transform from
/// a fixed distance.
///
class Camera : public SceneObject {
public:
	Camera(std::weak_ptr<GameEngine> engine, const std::string& name);
	~Camera() = default;

	// Override functions from SceneObject class
	virtual void PhysicsUpdate() override;
	virtual void Render(const std::shared_ptr<ShaderProgram> shader) const override;

	// Getters
	const glm::mat4& GetProjectionMtx() const;
	const glm::mat4& GetViewMtx() const;

	// Setters
	void SetFovDegrees(const float new_fov);
	void SetFovRadians(const float new_fov);
	void SetAspectRatio(const float new_aspect);
	void SetArmLength(const float new_length);
	void SetArmAngleDegrees(const glm::vec2 new_angle);
	void SetArmAngleRadians(const glm::vec2 new_angle);

	// Player Inputs
	void ApplyRotationInput(const glm::vec2& input);

private:
	// Vertical FOV of the camera in radians
	float fovY = glm::radians(45.0);
	// Closest/Farthest distance that this camera will render
	float clipNear = 0.1f;
	float clipFar = 100.0f;
	// Aspect ratio of the window that is rendering this camera's view (width/height)
	float aspectRatio = 1.0f;
	// Multiplier applied to mouse inputs
	float rotSpeed = 0.005f;
	// How far should the camera be from its parent as it rotates?
	float armLength = 5.0f;
	// Rotation of the camera's arm, relative to the camera's parent
	//   X = rotation around the camera's parent's RIGHT axis
	//   Y = rotation around the camera's parent's UP axis
	// Point the rotation axis away from you, angle+ is CW rotation on the resulting plane
	//   ex: armAngle.y = pi/4 means it's rotated halfway between +X axis and +Z axis
	glm::vec2 armAngle = glm::vec2(0.0f);
	// Maximum value of armAngle.x (must be LESS than pi/2)
	const float maxVerticalAngle = (glm::pi<float>() / 2.0f) - 0.5f;

	// Store the projection and view matrices on this camera object
	glm::mat4 projectionMtx;
	// Note: the view matrix is stored in WORLD space, so it can be accessed directly
	//   by shaders
	glm::mat4 viewMtx;
	void UpdateProjectionMtx();
	void UpdateViewMtx();
};
