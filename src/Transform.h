#pragma once

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

/// 
/// Helper class for defining transforms and misc transform operations
///
class Transform {
public:
	Transform() :
		loc(0.0f, 0.0f, 0.0f),
		rot(0.0f, 0.0f, 0.0f),
		scale(1.0f, 1.0f, 1.0f) {}
	Transform(glm::vec3 in_loc, glm::vec3 in_rot, glm::vec3 in_scale) :
		loc(in_loc), rot(in_rot), scale(in_scale) {}
	~Transform() = default;

	glm::mat4 GetMatrix() {
		glm::mat4 M(1.0f);
		M = glm::translate(M, loc);
		M = glm::rotate(M, rot.x, glm::vec3(1.0f, 0.0f, 0.0f));
		M = glm::rotate(M, rot.y, glm::vec3(0.0f, 1.0f, 0.0f));
		M = glm::rotate(M, rot.z, glm::vec3(0.0f, 0.0f, 1.0f));
		M = glm::scale(M, scale);
		return M;
	}

	// loc, rot, and scale w.r.t a parent object
	glm::vec3 loc;
	// TODO: convert all euler rots to quats
	glm::vec3 rot;
	glm::vec3 scale;

	friend std::ostream& operator<<(std::ostream& os, const Transform& t);
private:
};

inline std::ostream& operator<<(std::ostream& os, const Transform& t) {
	os << "Loc: (" << t.loc.x << ", " << t.loc.y << ", " << t.loc.z << ")\n";
	os << "Rot: (" << t.rot.x << ", " << t.rot.y << ", " << t.rot.z << ")\n";
	os << "Scale: (" << t.scale.x << ", " << t.scale.y << ", " << t.scale.z << ")\n";
	return os;
}