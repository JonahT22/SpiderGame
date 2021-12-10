#include "SpiderCharacter.h"

#include <iostream>
#include <cassert>

#include <glm/glm.hpp>

#include "GameEngine.h"
#include "ShaderProgram.h"
#include "Transform.h"
#include "IK/IKChain.h"
#include "IK/LegTarget.h"

SpiderCharacter::SpiderCharacter(std::weak_ptr<GameEngine> engine, const std::string& name,
	const float move_speed, const float turn_speed,
	const size_t legs_per_side, const size_t links_per_chain,
	const glm::vec3 leg_pos, const glm::vec3 target_pos,
	const bool render_links, const bool render_leg_targets,
	const float target_threshold, const float target_lerp_time) :
	SceneObject(engine, name),
	moveSpeed(move_speed), turnSpeed(turn_speed),
	legsPerSide(legs_per_side), linksPerChain(links_per_chain),
	legPos(leg_pos), targetPos(target_pos),
	renderLinks(render_links), renderLegTargets(render_leg_targets),
	targetThreshold(target_threshold), targetLerpTime(target_lerp_time)
{}

void SpiderCharacter::BeginPlay() {
	// Create the legs
	const std::string sides[2] = { "L", "R" };
	const float signs[2] = { 1.0f, -1.0f };
	for (size_t i = 0; i < legsPerSide; ++i) {
		// Space the leg depths (z) evenly over the spider's body
		float z_alpha;
		if (legsPerSide != 1) {
			z_alpha = i / (float)(legsPerSide - 1);	
		}
		else {
			// Avoid potential div by 0 when there is only 1 leg
			z_alpha = 0.5f;
		}
		float leg_z = ((1.0f - z_alpha) * legPos.z) + (z_alpha * -1.0f * legPos.z);
		// Also space the leg targets evenly
		float target_z = ((1.0f - z_alpha) * targetPos.z) + (z_alpha * -1.0f * targetPos.z);
		// Right and left legs
		for (size_t j = 0; j < 2; ++j) {
			// Name has format 'leg_L_0', 'leg_R_1_target' etc.
			auto new_leg_chain = std::make_shared<IKChain>(engineRef,
				"leg_" + sides[j] + "_" + std::to_string(i),
				linksPerChain, renderLinks);
			auto new_leg_target = std::make_shared<LegTarget>(engineRef,
				"leg_" + sides[j] + "_" + std::to_string(i) + "_target",
				renderLegTargets, targetThreshold, targetLerpTime);
			new_leg_chain->SetRelativeLocation(glm::vec3(signs[j] * legPos.x, legPos.y, leg_z));
			// Legs on the right should be flipped
			if (j == 1) {
				new_leg_chain->SetRelativeRotationDegrees(glm::vec3(0.0f, 180.0f, 0.0f));
			}
			new_leg_target->SetRelativeLocation(glm::vec3(signs[j] * targetPos.x, targetPos.y, target_z));

			// Add the new chain & target to this object
			legList.emplace_back(std::make_pair(new_leg_chain, new_leg_target));
			AddChildObject(new_leg_chain);
			AddChildObject(new_leg_target);
		}
	}

	// Set the 'neighbor' refs for the LegTargets
	// Order placed in the legList: L1, R1, L2, R2, L3, R3, etc...
	size_t end = legList.size() - 1;
	// Front 2 legs
	MakeLegNeighbors(0, 1);
	// Back 2 legs
	MakeLegNeighbors(end, end - 1);
	// Middle neighbors. Work from the front of the spider backward
	for (size_t i = 0; i + 3 < legList.size(); i += 2) {
		// Left-side neighbors
		MakeLegNeighbors(i, i + 2);
		// Right-side neighbors
		MakeLegNeighbors(i + 1, i + 3);
	}

	// Manually call BeginPlay on child objects, since they aren't being managed by the Scene
	for (auto& child : childObjects) {
		child.lock()->BeginPlay();
	}
	
	// Mark physics dirty to trigger the legs to start calculating their positions
	//   on the first physicsupdate
	MarkPhysicsDirty();
}

void SpiderCharacter::PhysicsUpdate(const float delta_time) {
	// Rotation from input
	rootTransform.AddRotationOffset(GetAngularSpeed() * delta_time,
	                                glm::vec3(0.0f, 1.0f, 0.0f));
	rootTransform.loc += GetLinearVelocity() * delta_time;
	MarkPhysicsDirty();
	SceneObject::PhysicsUpdate(delta_time);
}

void SpiderCharacter::Render(const std::shared_ptr<ShaderProgram> shader) const {
	SceneObject::Render(shader);
	// Manually call Render on child objects, since they aren't being managed by the Scene
	for (auto& child : childObjects) {
		child.lock()->Render(shader);
	}
}

glm::vec3 SpiderCharacter::GetLinearVelocity() const {
	auto engine = engineRef.lock();
	glm::vec3 total_velocity(0.0f);
	// Foward/Backward input
	if (engine->IsKeyPressed(GLFW_KEY_W)) {
		total_velocity += rootTransform.GetForwardVector();
	}
	else if (engine->IsKeyPressed(GLFW_KEY_S)) {
		total_velocity -= rootTransform.GetForwardVector();
	}
	// Side to side input
	if (engine->IsKeyPressed(GLFW_KEY_A)) {
		total_velocity += rootTransform.GetRightVector();
	}
	else if (engine->IsKeyPressed(GLFW_KEY_D)) {
		total_velocity -= rootTransform.GetRightVector();
	}
	total_velocity *= moveSpeed;
	return total_velocity;
}

float SpiderCharacter::GetAngularSpeed() const {
	auto engine = engineRef.lock();
	// Rotation input
	if (engine->IsKeyPressed(GLFW_KEY_RIGHT)) {
		return -1.0f * turnSpeed;
	}
	else if (engine->IsKeyPressed(GLFW_KEY_LEFT)) {
		return turnSpeed;
		
	}
	else return 0.0f;
}

inline void SpiderCharacter::MakeLegNeighbors(size_t index_1, size_t index_2) {
	assert(index_1 < legList.size() && index_2 < legList.size());
	legList.at(index_1).second->AddNeighbor(legList.at(index_2).second);
	legList.at(index_2).second->AddNeighbor(legList.at(index_1).second);
}
