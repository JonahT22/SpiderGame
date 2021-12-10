#include "SpiderCharacter.h"

#include <iostream>
#include <cassert>

#include <glm/glm.hpp>

#include "GameEngine.h"
#include "ShaderProgram.h"
#include "Transform.h"
#include "IK/IKChain.h"
#include "IK/LegTarget.h"

SpiderCharacter::SpiderCharacter(std::weak_ptr<GameEngine> engine, const std::string& name) :
	SceneObject(engine, name) {}

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
		float leg_z = ((1.0f - z_alpha) * maxLegZ) + (z_alpha * -1.0f * maxLegZ);
		// Also space the leg targets evenly
		float target_z = ((1.0f - z_alpha) * maxTargetZ) + (z_alpha * -1.0f * maxTargetZ);
		// Right and left legs
		for (size_t j = 0; j < 2; ++j) {
			// Name has format 'leg_L_0', 'leg_R_1_target' etc.
			auto new_leg_chain = std::make_shared<IKChain>(engineRef,
				"leg_" + sides[j] + "_" + std::to_string(i));
			auto new_leg_target = std::make_shared<LegTarget>(engineRef,
				"leg_" + sides[j] + "_" + std::to_string(i) + "_target");
			new_leg_chain->SetRelativeLocation(glm::vec3(signs[j] * legX, legY, leg_z));
			// Legs on the right should be flipped
			if (j == 1) {
				new_leg_chain->SetRelativeRotationDegrees(glm::vec3(0.0f, 180.0f, 0.0f));
			}
			new_leg_target->SetRelativeLocation(glm::vec3(signs[j] * targetX, targetY, target_z));

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
	
	return glm::vec3();
}

inline void SpiderCharacter::MakeLegNeighbors(size_t index_1, size_t index_2) {
	assert(index_1 < legList.size() && index_2 < legList.size());
	legList.at(index_1).second->AddNeighbor(legList.at(index_2).second);
	legList.at(index_2).second->AddNeighbor(legList.at(index_1).second);
}
