#pragma once

#include <iostream>
#include <string>
#include <stdexcept>

#include <glm/glm.hpp>
#include <yaml-cpp/yaml.h>

#include "YAMLHelper.h"

class GameOptions {
public:
	GameOptions() = default;
	GameOptions(const std::string& filename) {
		try {
			// Load the options file
			YAML::Node options_node = YAML::LoadFile(filename);
			windowWidth = YAMLHelper::GetMapVal<int>(options_node, "window_width");
			windowHeight = YAMLHelper::GetMapVal<int>(options_node, "window_height");
			windowName = YAMLHelper::GetMapVal<std::string>(options_node, "window_name");
			clearColor = YAMLHelper::GetMapVal<glm::vec3>(options_node, "clear_color");
			physicsTimeStep = 1.0 / YAMLHelper::GetMapVal<float>(options_node, "physics_fps");
			frameDelayMs = YAMLHelper::GetMapVal<unsigned int>(options_node, "frame_delay_ms");
			showFramerate = YAMLHelper::GetMapVal<bool>(options_node, "show_frame_rate");
			defaultModelPath = YAMLHelper::GetMapVal<std::string>(options_node,"default_model_path");
		}
		catch (std::exception& e) {
			std::cerr << "ERROR - YAML parsing exception: " << e.what() << std::endl;
		}
	}
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