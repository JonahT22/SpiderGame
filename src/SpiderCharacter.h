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
	// TODO: this is too many parameters, back into structs for ChainSettings and
	//   LegSettings
	SpiderCharacter(std::weak_ptr<GameEngine> engine, const std::string& name,
		const float move_speed, const float turn_speed,
		const size_t legs_per_side, const size_t links_per_chain,
		const glm::vec3 leg_pos, const glm::vec3 target_pos,
		const bool render_links, const bool render_leg_targets,
		const float target_threshold, const float target_lerp_time);
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

	// Settings for generating legs and legtargets
	const size_t legsPerSide = 3;
	const size_t linksPerChain = 2;
	const glm::vec3 legPos = glm::vec3(0.1f, 0.23f, 0.3f);
	const glm::vec3 targetPos = glm::vec3(0.8f, -0.2f, 0.7f);
	const bool renderLinks = true;
	const bool renderLegTargets = false;
	const float targetThreshold = 0.6;
	const float targetLerpTime = 0.1;

	// Keep a list of legs & target that the SpiderObject controls (NOT controlled by the scene)
	std::vector<std::pair<std::shared_ptr<IKChain>, std::shared_ptr<LegTarget> > > legList;
	// Distance to cover per physics frame
	const float moveSpeed = 2.0f;
	// Amount to rotate about this object's y axis per frame
	const float turnSpeed = 1.2f;
};

