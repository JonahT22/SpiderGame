#include <string>
#include <stdexcept>
#include <iostream>

#include <glm/glm.hpp>
#include <yaml-cpp/yaml.h>

#include "GameOptions.h"
#include "YAMLHelper.h"

// TODO: move this implementation back to an inline dec in the header?

GameOptions::GameOptions(const std::string& filename) {
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
		defaultModelPath = YAMLHelper::GetMapVal<std::string>(options_node, "default_model_path");
	}
	catch (std::exception& e) {
		std::cerr << "ERROR - YAML parsing exception: " << e.what() << std::endl;
	}
}