// Include order: std library, external libraries, project headers
#include <memory>
#include <iostream>

#include <glad/glad.h>
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
	GLfloat prev_time = glfwGetTime();
	GLfloat current_time, delta_time;
	while (spider_game->IsWindowOpen()) {
		current_time = glfwGetTime();
		delta_time = current_time - prev_time;
		prev_time = current_time;
		
		// Render the entire scene
		spider_game->RenderScene(delta_time);
	}
	return 0;
}