#include "Camera.h"

#include <iostream>

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/type_ptr.hpp>

Camera::Camera() :
	fovY(glm::radians(45.0)),
	clipNear(0.1f),
	clipFar(100.0f),
	transform(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f)),
	rotSpeed(0.005)
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

void Camera::SetLocation(const glm::vec3& new_location) {
	transform.loc = new_location;
	UpdateViewMtx();
}

void Camera::SetRotation(const glm::vec3& new_rotation) {
	transform.rot = new_rotation;
	UpdateViewMtx();
}

void Camera::SetScale(const glm::vec3& new_scale) {
	transform.scale = new_scale;
	UpdateViewMtx();
}

void Camera::ApplyRotationInput(const glm::vec2& input) {
	// Note: inputs are in screen-space (x = horizontal, y = vertical).
	//   So, x-inputs should rotate around the y (vertical) axis, and vice versa
	// Also, subtract the inputs so that positive mouse inputs result in
	//   CCW (negative) rotations
	transform.rot.y -= input.x * rotSpeed;
	transform.rot.x -= input.y * rotSpeed;
	UpdateViewMtx();
}

void Camera::UpdateProjectionMtx() {
	projectionMtx = glm::perspective(fovY, aspectRatio, clipNear, clipFar);
}

void Camera::UpdateViewMtx() {
	// TODO: There's probably a way to shortcut the inverse of a transformation matrix
	viewMtx = glm::inverse(transform.GetMatrix());
}
