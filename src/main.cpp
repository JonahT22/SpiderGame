// Include order: std library, external libraries, project headers
#include <memory>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "GameEngine.h"


int main(int argc, char** argv) {
	if (argc != 2) {
		std::cerr << "Usage: <SCENE_FILE>" << std::endl;
		return 1;
	}
	char* scene_file = argv[1];

	/* ----- Create the game instance & main rendering window ----- */
	// Use default options
	GameOptions game_options;
	auto spider_game = std::make_shared<GameEngine>(game_options);

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