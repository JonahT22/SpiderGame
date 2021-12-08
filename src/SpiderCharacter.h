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

	virtual void BeginPlay() override;
	virtual void PhysicsUpdate() override;
	virtual void Render(const std::shared_ptr<ShaderProgram> shader) const override;

private:
	// Distance to cover per physics frame
	// TODO: make this a proper speed affected by physicstickrate
	const float moveSpeed = 0.1f;
	// Amount to rotate about this object's y axis per frame
	// TODO: make this a proper angular velocity affected by physicstickrate
	const float turnSpeed = 0.05f;
};

