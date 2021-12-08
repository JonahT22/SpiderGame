#include "SpiderCharacter.h"

#include <iostream>

#include <glm/glm.hpp>

#include "GameEngine.h"
#include "ShaderProgram.h"
#include "Transform.h"

SpiderCharacter::SpiderCharacter(std::weak_ptr<GameEngine> engine, const std::string& name) :
	SceneObject(engine, name) {

}

void SpiderCharacter::PhysicsUpdate() {
	auto engine = engineRef.lock();
	// Rotation input
	if (engine->IsKeyPressed(GLFW_KEY_RIGHT)) {
		rootTransform.AddRotationOffset(-1.0f * turnSpeed, glm::vec3(0.0f, 1.0f, 0.0f));
	}
	else if (engine->IsKeyPressed(GLFW_KEY_LEFT)) {
		rootTransform.AddRotationOffset(turnSpeed, glm::vec3(0.0f, 1.0f, 0.0f));
	}
	// Foward/Backward input
	if (engine->IsKeyPressed(GLFW_KEY_W)) {
		rootTransform.loc += rootTransform.GetForwardVector() * moveSpeed;
	}
	else if (engine->IsKeyPressed(GLFW_KEY_S)) {
		rootTransform.loc -= rootTransform.GetForwardVector() * moveSpeed;
	}
	// Side to side input
	if (engine->IsKeyPressed(GLFW_KEY_A)) {
		rootTransform.loc += rootTransform.GetRightVector() * moveSpeed;
	}
	else if (engine->IsKeyPressed(GLFW_KEY_D)) {
		rootTransform.loc -= rootTransform.GetRightVector() * moveSpeed;
	}
	MarkPhysicsDirty();
	SceneObject::PhysicsUpdate();
}

void SpiderCharacter::Render(const std::shared_ptr<ShaderProgram> shader) const {
	SceneObject::Render(shader);
}
