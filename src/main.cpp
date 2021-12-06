#include <memory>
#include <iostream>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "GameEngine.h"

int main() {
	/* ----- Create the game instance & main rendering window ----- */
	GameOptions game_options{ 800, 600, "Spider Game" };
	auto spider_game = std::make_shared<GameEngine>(game_options);

	/* ----- Load the Scene Geometry ----- */
	spider_game->SetupScene("resources/scenes/testscene.yaml");

	/* ----- Render Loop ----- */
	float prev_time = glfwGetTime();
	float current_time, delta_time;
	while (spider_game->IsWindowOpen()) {
		current_time = glfwGetTime();
		delta_time = current_time - prev_time;
		prev_time = current_time;
		
		// Render the entire scene
		spider_game->RenderScene(delta_time);
	}
	return 0;
}