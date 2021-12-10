#pragma once

#include <memory>
#include <string>

#include <glm/glm.hpp>

#include "../SceneObject.h"
class ShaderProgram;
class GameEngine;
class Mesh;

///
/// Empty SceneObject that lazily updates its model matrix location to match its
/// relative transform. Rotation & scale changes are immediately updated, but location
/// changes are only applied to the model matrix if they fall outside a certain radius
/// from the relative transform's world position
///
class LegTarget : public SceneObject {
public:
	LegTarget(std::weak_ptr<GameEngine> engine, const std::string& name,
		const bool viz_mesh, const float threshold, const float lerp_time);
	~LegTarget() = default;

	virtual void BeginPlay() override;
	virtual void PhysicsUpdate(const float delta_time) override;
	virtual void Render(const std::shared_ptr<ShaderProgram> shader) const override;

	// Getters
	bool IsMoving() const;

	// Setters
	void AddNeighbor(std::weak_ptr<LegTarget> new_neighbor);

private:
	// TODO: organize the parameters and pass in through file
	const bool visualizeMesh = false;
	std::shared_ptr<Mesh> vizMesh;

	// Reference to any adjacent LegTargets. This LegTarget should not start moving
	//   if any of its neighbors are also moving
	std::vector<std::weak_ptr<LegTarget> > neighbors;
	
	// World-space radius threshold before the lazy location decides to catch up
	//   to actual location
	const float threshold = 0.6f;
	// Amount of time to lerp between the 'old' and 'new' target points when the goal
	//   distance exceed the threshold
	const float lerpTimeLength = 0.1f;
	// Amount that the leg should lift in the air when moving from point-to-point
	const float legLiftHeight = 0.2f;
	// Multiplier to scale the velocity of the spider when offsetting the goal location
	const float velocityFactor = 0.25f;
	// Current amount of time that the target point has been lerping to the goal
	float lerpTimer = 0.0f;
	// 'Old' location of the target, captured when it begins to move to the goal
	glm::vec4 prevLoc = glm::vec4(0, 0, 0, 1);
	// Tracks whether the leg is currently lerping to the goal point
	bool isLegMoving = false;
	// If one LegTarget is always moving, it's neighbors will be starved. Add this lock
	//   to prevent a LegTarget from moving again immediately after reaching its target
	bool justFinishedMoving = false;
	// Store the value of Pi for easy reference
	static constexpr float PI = glm::pi<float>();
};

