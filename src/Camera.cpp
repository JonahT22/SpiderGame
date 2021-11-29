#include "Camera.h"

#include <iostream>

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/type_ptr.hpp>

Camera::Camera() :
	fovY(glm::radians(45.0)),
	clipNear(0.1f),
	clipFar(100.0f),
	transform(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f)),
	rotSpeed(0.005),
	armLength(5.0f),
	armAngle(glm::vec2(0.0f)),
	maxVerticalAngle((glm::pi<float>() / 2.0) - 0.05)
{
	UpdateProjectionMtx();
	UpdateViewMtx();
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
	// Note: inputs are in screen-space (x = horizontal, y = vertical).
	//   So, x-inputs should rotate around the y (vertical) axis, and vice versa
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
	// Note: if armangles are specified w.r.t a
	// Set the camera's location based on its arm length/angle
	transform.loc.x = armLength * cos(armAngle.x) * cos(armAngle.y);
	transform.loc.y = armLength * sin(armAngle.x);
	transform.loc.z = armLength * cos(armAngle.x) * sin(armAngle.y);

	// Note: this assumes that the camera is not parented to any objects, and
	//   that it is always looking torward the world origin
	viewMtx = glm::lookAt(transform.loc, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
}
