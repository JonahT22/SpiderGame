#pragma once

#include <string>
#include <stdexcept>

#include <glm/glm.hpp>

class GameOptions {
public:
	GameOptions() = default;
	GameOptions(const std::string& filename);
	~GameOptions() = default;

	// TODO: move defaults to the cpp file
	int windowWidth = 800;
	int windowHeight = 600;
	std::string windowName = "Spider Game";
	glm::vec3 clearColor = glm::vec3(0.1f, 0.1f, 0.1f);
	// Desired period for the physics updates
	float physicsTimeStep = 1.0f / 60.0f;
	// Artificial delay on each rendering frame, for testing physics behavior
	unsigned int frameDelayMs = 0;
	// Should the framerate be printed to stdout?
	bool showFramerate = false;
	std::string defaultModelPath = "";
};