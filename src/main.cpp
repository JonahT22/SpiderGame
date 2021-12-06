#include <memory>
#include <iostream>
#include <string>

// TODO: I shouldn't need to include glad and glfw here, should be abstracted
// behind helper classes
// Always include glad before glfw
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ShaderProgram.h"
#include "GameEngine.h"
#include "Mesh.h"
// TODO: this shouldn't be necessary
#include "Texture.h"
// TODO: this shouldn't be necessary
#include "Camera.h"
// TODO: remove
#include "Window.h"

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