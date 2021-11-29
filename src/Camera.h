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
	// TODO: temporary
	const glm::mat4 GetProjectionMtx() const { return projectionMtx; }
	const glm::mat4 GetViewMtx() const { return viewMtx; }

	// Setters
	void SetFovDegrees(const float new_fov);
	void SetFovRadians(const float new_fov);
	void SetAspectRatio(const float new_aspect);
	void SetLocation(const glm::vec3& new_location);
	void SetRotation(const glm::vec3& new_rotation);
	void SetScale(const glm::vec3& new_scale);

	// Player Inputs

private:
	// Vertical FOV of the camera in radians
	float fovY;
	// Closest/Farthest distance that this camera will render
	float clipNear;
	float clipFar;
	// Aspect ratio of the window that is rendering this camera's view (width/height)
	float aspectRatio;
	// TODO: is it okay to have a scale transform on a camera?
	Transform transform;

	// Store the projection and view matrices on this camera object
	glm::mat4 projectionMtx;
	glm::mat4 viewMtx;
	void UpdateProjectionMtx();
	void UpdateViewMtx();
};
