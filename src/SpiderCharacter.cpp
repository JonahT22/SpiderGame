#include "SpiderCharacter.h"

#include <iostream>

#include "ShaderProgram.h"
#include "GameEngine.h"

SpiderCharacter::SpiderCharacter(std::weak_ptr<GameEngine> engine, const std::string& name) :
	SceneObject(engine, name) {

}

void SpiderCharacter::PhysicsUpdate() {
	auto engine = engineRef.lock();
	// Rotation input
	if (engine->IsKeyPressed(GLFW_KEY_RIGHT)) {
		rootTransform.AddRotationOffset(turnSpeed, glm::vec3(0.0f, -1.0f, 0.0f));
	}
	else if (engine->IsKeyPressed(GLFW_KEY_LEFT)) {
		rootTransform.AddRotationOffset(-1.0f * turnSpeed, glm::vec3(0.0f, -1.0f, 0.0f));
	}
	MarkPhysicsDirty();
	SceneObject::PhysicsUpdate();
}

void SpiderCharacter::Render(const std::shared_ptr<ShaderProgram> shader) const {
	SceneObject::Render(shader);
}
