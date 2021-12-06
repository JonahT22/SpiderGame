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

	/* ----- Create the camera ----- */
	auto main_camera = std::make_shared<Camera>(spider_game, "main_camera");
	main_camera->SetAspectRatio(game_options.windowWidth / (float)game_options.windowHeight);
	main_camera->SetArmLength(5.0f);
	main_camera->SetArmAngleDegrees(glm::vec2(30.0f, 90.0f));
	spider_game->SetCurrentCamera(main_camera);

	/* ----- Load the Scene Geometry ----- */
	spider_game->SetupScene("asdf");

	/* ----- Render Loop ----- */
	// has GLFW been told to close?
	// TODO: badly worded
	while (!glfwWindowShouldClose(spider_game->GetWindow()->GetWindow())) {
		// Set the void color to dark green-blue (STATE-SETTING function)
		glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
		// Clear the color & depth buffers (STATE-USING function)
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		GLfloat time = glfwGetTime();
		
		// Render the entire scene
		spider_game->RenderScene();

		// check and call events, swap buffers
		glfwSwapBuffers(spider_game->GetWindow()->GetWindow()); // swap the color buffers
		glfwPollEvents(); // have any events been triggered (i.e. input?)
	}
	return 0;
}