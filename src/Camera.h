#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Transform.h"

/// 
/// Controllable camera that always points toward the center of its parent object from
/// a fixed distance.
///
class Camera {
public:
	Camera();
	~Camera() = default;

	// Getters
	const GLfloat* GetProjectionMtxPtr() const;
	const GLfloat* GetViewMtxPtr() const;

	// Setters
	void SetFovDegrees(const float new_fov);
	void SetFovRadians(const float new_fov);
	void SetAspectRatio(const float new_aspect);
	void SetArmLength(const float new_length);
	void SetArmAngleDegrees(const glm::vec2 new_angle);
	void SetArmAngleRadians(const glm::vec2 new_angle);
	// Note: there are NO setters for loc, rot, or scale. These are controlled by the
	//   parent object & player inputs

	// Player Inputs
	void ApplyRotationInput(const glm::vec2& input);

private:
	// Vertical FOV of the camera in radians
	float fovY;
	// Closest/Farthest distance that this camera will render
	float clipNear;
	float clipFar;
	// Aspect ratio of the window that is rendering this camera's view (width/height)
	float aspectRatio;
	// Location, rotation, and scale of the camera. Controlled by the arm's length & angle
	Transform transform;
	// Multiplier applied to mouse inputs
	float rotSpeed;
	// How far should the camera be from its parent as it rotates?
	float fixedArmLength;
	// Rotation of the camera's arm, relative to the camera's parent
	//   X = rotation around the camera's parent's RIGHT axis
	//   Y = rotation around the camera's parent's UP axis
	glm::vec2 armAngle;
	// Maximum value of armAngle.x (must be LESS than pi/2)
	const float maxVerticalAngle;

	// Store the projection and view matrices on this camera object
	glm::mat4 projectionMtx;
	glm::mat4 viewMtx;
	void UpdateProjectionMtx();
	void UpdateViewMtx();
};
