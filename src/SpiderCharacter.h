#pragma once

#include <memory>
#include <string>

#include "SceneObject.h"
class ShaderProgram;
class GameEngine;

///
/// Controllable character
///
class SpiderCharacter : public SceneObject {
public:
	SpiderCharacter(std::weak_ptr<GameEngine> engine, const std::string& name);
	~SpiderCharacter() = default;

	virtual void PhysicsUpdate() override;
	virtual void Render(const std::shared_ptr<ShaderProgram> shader) const override;

private:
	// The character has exclusive control over all objects attached to it, 
};

