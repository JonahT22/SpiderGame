#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "SceneObject.h"
class ShaderProgram;
class GameEngine;
class IKChain;
class LegTarget;

///
/// Controllable character
///
class SpiderCharacter : public SceneObject {
public:
	SpiderCharacter(std::weak_ptr<GameEngine> engine, const std::string& name);
	~SpiderCharacter() = default;

	virtual void BeginPlay() override;
	virtual void PhysicsUpdate(const float delta_time) override;
	virtual void Render(const std::shared_ptr<ShaderProgram> shader) const override;

	// Query the user inputs to find what velocity the spider should be moving at
	glm::vec3 GetLinearVelocity() const;
	// Query the user inputs to find the angular speed (abt the y axis) that the
	//   spider should be moving at
	float GetAngularSpeed() const;

private:
	// Make 2 LegTargets in the legList neighbors of each other
	inline void MakeLegNeighbors(size_t index_1, size_t index_2);

	// TODO: read from file
	size_t legsPerSide = 3;
	float legY = 0.14;
	float legX = 0.25;
	float maxLegZ = 0.3;
	float targetX = 0.8;
	float targetY = -0.2;
	float maxTargetZ = 0.7;
	// Keep a list of legs & target that the SpiderObject controls (NOT controlled by the scene)
	std::vector<std::pair<std::shared_ptr<IKChain>, std::shared_ptr<LegTarget> > > legList;
	// Distance to cover per physics frame
	const float moveSpeed = 2.0f;
	// Amount to rotate about this object's y axis per frame
	const float turnSpeed = 1.2f;
};

