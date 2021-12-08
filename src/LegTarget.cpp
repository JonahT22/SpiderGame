#include "LegTarget.h"

#include <iostream>

#include <glad/glad.h>

#include "GameEngine.h"

LegTarget::LegTarget(std::weak_ptr<GameEngine> engine, const std::string& name) :
	SceneObject(engine, name), physicsTimeStep(engine.lock()->GetPhysicsTimeStep()) {}

void LegTarget::BeginPlay() {
	// Initialize the modelMtx with the full set of parent transforms, without
	//   interpolation
	if (!parent.expired()) {
		modelMtx = parent.lock()->GetWorldTransformMtx() * rootTransform.GetMatrix();
	}
}

void LegTarget::PhysicsUpdate() {
	if (physicsDirty) {
		// Find the world-space transform of the goal point
		glm::mat4 goalMtx = rootTransform.GetMatrix();
		if (!parent.expired()) {
			goalMtx = parent.lock()->GetWorldTransformMtx() * goalMtx;
		}

		// TODO: This 'goal location' is just the point that the leg needs to go to
		//   to reach it's resting position on the spider. Instead, take the spider's
		//   velocity into account to 'reach forward' and try to predict the spiders's
		//   motion

		// World-space location of the goal (keep as vec4 with w = 1)
		glm::vec4 goalLoc = goalMtx[3];

		// Always update rotation & scale in the model matrix, no matter what the
		//   location is. But leave translation (col 3) up to the interpolation code
		modelMtx[0] = goalMtx[0];
		modelMtx[1] = goalMtx[1];
		modelMtx[2] = goalMtx[2];
		// Mark physics clean, but it might be re-marked later if interpolation is needed
		physicsDirty = false;

		// If this leg is already moving, continue lerping between the prevLoc
		//   and goalLoc
		if (isLegMoving) {
			// Interpolation value, 0 = at prevLoc, 1 = at goalLoc
			float alpha = lerpTimer / lerpTimeLength;
			//std::cout << "Moving, alpha = " << alpha << std::endl;
			if (alpha < 1.0f) {
				// Still interpolating
				glm::vec4 interpLoc = ((1.0f - alpha) * prevLoc) + (alpha * goalLoc);
				// Offset the interp location by a vertical curve (for leg-lifting effect)
				// TODO: offset this in the spidercharacter's up vector, not y
				interpLoc.y += sin(alpha * PI) * legLiftHeight;
				modelMtx[3] = interpLoc;
				lerpTimer += physicsTimeStep;
				// Each update, re-mark physics dirty to propagate changes to children
				MarkPhysicsDirty();
			}
			else {
				isLegMoving = false;
			}
		}
		else {
			// Leg is not moving, so check whether it should start
			float distance = glm::length(goalLoc - modelMtx[3]);
			if (distance > threshold) {
				// Start moving
				isLegMoving = true;
				lerpTimer = 0.0f;
				// Save the location when the threshold was reached
				prevLoc = modelMtx[3];
			}
		}
	}

	// Propagate physics to children
	for (auto& child : childObjects) {
		if (!child.expired()) {
			child.lock()->PhysicsUpdate();
		}
		else {
			std::cerr << "ERROR: Attempted to update physics on an invalid ";
			std::cerr << "child object!" << std::endl;
		}
	}
}

void LegTarget::Render(const std::shared_ptr<ShaderProgram> shader) const {
	// LegTargets have no visual display, attach a mesh object to visualize
}
