#include "SpiderCharacter.h"

#include <iostream>

#include "ShaderProgram.h"

SpiderCharacter::SpiderCharacter(std::weak_ptr<GameEngine> engine, const std::string& name) :
	SceneObject(engine, name) {

}

void SpiderCharacter::PhysicsUpdate() {
	SceneObject::PhysicsUpdate();
}

void SpiderCharacter::Render(const std::shared_ptr<ShaderProgram> shader) const {
	SceneObject::Render(shader);
}
