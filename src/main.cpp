// Include order: std library, external libraries, project headers
#include <memory>
#include <iostream>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "GameEngine.h"

int main(int argc, char** argv) {
	if (argc != 3) {
		std::cerr << "Usage: <ENGINE_SETTINGS> <SCENE_FILE>" << std::endl;
		return 1;
	}
	char* engine_settings = argv[1];
	char* scene_file = argv[2];

	/* ----- Create the game instance & main rendering window ----- */
	auto spider_game = std::make_shared<GameEngine>(engine_settings);

	/* ----- Load the Scene Geometry ----- */
	spider_game->SetupScene(scene_file);

	/* ----- Render Loop ----- */
	double prev_time = glfwGetTime();
	double current_time, delta_time;
	while (spider_game->IsWindowOpen()) {
		current_time = glfwGetTime();
		delta_time = current_time - prev_time;
		prev_time = current_time;
		
		// Render the entire scene
		spider_game->RenderScene(delta_time);
	}
	return 0;
}