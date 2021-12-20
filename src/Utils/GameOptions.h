#pragma once

#include <string>
#include <stdexcept>

#include <glm/glm.hpp>

class GameOptions {
public:
	GameOptions(const std::string& filename);
	~GameOptions() = default;

	int windowWidth = 800;
	int windowHeight = 600;
	std::string windowName = "Spider Game";
	glm::vec3 clearColor = glm::vec3(0.1f, 0.1f, 0.1f);
	float physicsTimeStep = 1.0f / 60.0f;
	unsigned int frameDelayMs = 0;
	bool showFramerate = false;
};