#include "Camera.h"

#include <iostream>

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_access.hpp>

Camera::Camera() :
	fovY(glm::radians(45.0)),
	clipNear(0.1f),
	clipFar(100.0f),
	rotSpeed(0.005),
	armLength(5.0f),
	armAngle(glm::vec2(0.0f)),
	maxVerticalAngle((glm::pi<float>() / 2.0) - 0.05)
{
	UpdateProjectionMtx();
	UpdateViewMtx();
}

void Camera::PhysicsUpdate(const glm::mat4& parent_transform) {
	// Do the same as the SceneObject physicsupdate, but also update the view
	//   matrix afterward to get any updates to this camera's transform
	SceneObject::PhysicsUpdate(parent_transform);
	UpdateViewMtx();
}

void Camera::Render(const std::shared_ptr<ShaderProgram> shader) {
	// TODO: define a quick visualization for cameras, but don't draw this
	//   camera if it's the main camera being referenced by the GameEngine
}

const GLfloat* Camera::GetProjectionMtxPtr() const {
	return glm::value_ptr(projectionMtx);
}

const GLfloat* Camera::GetViewMtxPtr() const {
	return glm::value_ptr(viewMtx);
}

void Camera::SetFovDegrees(const float new_fov) {
	fovY = glm::radians(new_fov);
	UpdateProjectionMtx();
}

void Camera::SetFovRadians(const float new_fov) {
	fovY = new_fov;
	UpdateProjectionMtx();
}

void Camera::SetAspectRatio(const float new_aspect) {
	aspectRatio = new_aspect;
	UpdateProjectionMtx();
}

void Camera::SetArmLength(const float new_length) {
	armLength = new_length;
	UpdateViewMtx();
}

void Camera::SetArmAngleDegrees(const glm::vec2 new_angle) {
	armAngle.x = glm::radians(new_angle.x);
	armAngle.y = glm::radians(new_angle.y);
	UpdateViewMtx();
}

void Camera::SetArmAngleRadians(const glm::vec2 new_angle) {
	armAngle = new_angle;
	UpdateViewMtx();
}

void Camera::ApplyRotationInput(const glm::vec2& input) {
	// Note: inputs are provided in screen-space
	//   So, x-inputs should rotate around the y (vertical) axis, and
	//   y-inputs should rotate around the x (horizontal) axis
	// Also, subtract the inputs so that positive mouse inputs result in
	//   CCW (negative) rotations
	armAngle.y -= input.x * rotSpeed;
	armAngle.x -= input.y * rotSpeed;

	// Clamp vertical rotations
	if (armAngle.x >= maxVerticalAngle) {
		armAngle.x = maxVerticalAngle;
	}
	else if (armAngle.x <= -1.0f * maxVerticalAngle) {
		armAngle.x = -1.0f * maxVerticalAngle;
	}
	UpdateViewMtx();
}

void Camera::UpdateProjectionMtx() {
	projectionMtx = glm::perspective(fovY, aspectRatio, clipNear, clipFar);
}

void Camera::UpdateViewMtx() {
	// Note: there are 3 coordinate spaces in play here.
	//   World space: coordinate space of the scene
	//   Local space: coordinate space of the rootComponent, set by the rootTransform
	//     inherited from the SceneObject class
	//   Orbit space: coordinate space of the camera view, set by the armLength and
	//     armAngle
	// To get the viewMtx, we need a transformation matrix from world -> orbit space

	// Set the camera's orbit-space location based on the arm length & rotation
	glm::vec3 viewPos;
	viewPos.x = armLength * cos(armAngle.x) * cos(armAngle.y);
	viewPos.y = armLength * sin(armAngle.x);
	viewPos.z = armLength * cos(armAngle.x) * sin(armAngle.y);
	
	// TODO: check if roll rotations are handled, might need to also adjust this
	//   up vector
	// Using the local camera location, find a transformation matrix from the
	//   the rootComponent space -> camera's orbit view space
	glm::mat4 orbitMtx;
	orbitMtx = glm::lookAt(viewPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	
	// Find the total transformation matrix from world space -> camera orbit view space.
	// Note that this is the opposite order from going local -> world, since
	//   the view matrix defines a world -> local transformation
	viewMtx = orbitMtx * glm::inverse(modelMtx);
}
