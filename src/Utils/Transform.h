#pragma once

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

/// 
/// Helper class for defining transforms and misc transform operations
///
class Transform {
public:
	inline static glm::quat EulerToQuat(const glm::vec3 euler) {
		glm::quat q_x = glm::angleAxis(euler.x, glm::vec3(1.0f, 0.0f, 0.0f));
		glm::quat q_y = glm::angleAxis(euler.y, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::quat q_z = glm::angleAxis(euler.z, glm::vec3(0.0f, 0.0f, 1.0f));
		return q_x * q_y * q_z;
	}

	Transform() :
		loc(0.0f, 0.0f, 0.0f),
		// Note: quat constructor is (w, x, y, z)
		rot(1.0f, 0.0f, 0.0f, 0.0f),
		scale(1.0f, 1.0f, 1.0f) {}
	Transform(glm::vec3 in_loc, glm::quat in_rot, glm::vec3 in_scale) :
		loc(in_loc), rot(in_rot), scale(in_scale) {}
	Transform(glm::vec3 in_loc, glm::vec3 in_rot, glm::vec3 in_scale) :
		loc(in_loc), rot(EulerToQuat(in_rot)), scale(in_scale) {}
	~Transform() = default;

	// loc, rot, and scale w.r.t a parent object
	glm::vec3 loc;
	glm::quat rot;
	glm::vec3 scale;

	glm::mat4 GetMatrix() const {
		glm::mat4 M(1.0f);
		M = glm::translate(M, loc);
		M *= glm::mat4_cast(rot);
		M = glm::scale(M, scale);
		return M;
	}

	glm::vec3 GetRightVector() const {
		// Right vector is stored in the 1st column of the rotation matrix
		return glm::mat4_cast(rot)[0];
	}

	glm::vec3 GetUpVector() const {
		// Up vector is stored in the 2nd column of the rotation matrix
		return glm::mat4_cast(rot)[1];
	}

	glm::vec3 GetForwardVector() const {
		// Forward vector is stored in the 3rd column of the rotation matrix
		return glm::mat4_cast(rot)[2];
	}

	void AddRotationOffset(const float angle, const glm::vec3& axis) {
		rot = glm::rotate(rot, angle, axis);
	}

	friend std::ostream& operator<<(std::ostream& os, const Transform& t);
private:
};

inline std::ostream& operator<<(std::ostream& os, const Transform& t) {
	os << "Loc: (" << t.loc.x << ", " << t.loc.y << ", " << t.loc.z << ")\n";
	os << "Rot: (" << t.rot.x << ", " << t.rot.y << ", " << t.rot.z << ", " << t.rot.w << ")\n";
	os << "Scale: (" << t.scale.x << ", " << t.scale.y << ", " << t.scale.z << ")\n";
	return os;
}