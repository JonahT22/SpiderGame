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

int main() {
	/* ----- Create the game instance & main rendering window ----- */
	GameOptions game_options{ 800, 600, "Spider Game" };
	GameEngine spider_game(game_options);

	/* ----- Create scene geometry ----- */
	auto basic_shader = std::make_shared<ShaderProgram>();
	basic_shader->Compile("resources/simple_vert.glsl", "resources/simple_frag.glsl");
	TextureOptions tex_options{ GL_MIRRORED_REPEAT, GL_LINEAR_MIPMAP_LINEAR,
								GL_LINEAR, GL_RGB, GL_RGBA };
	auto cube1 = std::make_shared<Mesh>();
	cube1->GenerateCubeMesh();
	cube1->LoadTexture("resources/awesomeface.png", tex_options);
	cube1->SetRelativeLocation(glm::vec3(0.0f));

	auto cube2 = std::make_shared<Mesh>();
	cube2->GenerateCubeMesh();
	cube2->LoadTexture("resources/awesomeface.png", tex_options);
	cube2->SetRelativeLocation(glm::vec3(2.0f, 0.0f, 0.0f));
	cube2->SetRelativeRotationDegrees(glm::vec3(0.0f, 45.0f, 30.0f));
	cube2->SetRelativeScale(glm::vec3(1.0f, 0.5f, 0.5f));
	
	cube1->AddChildObject(cube2);
	cube1->PhysicsUpdate(glm::mat4(1.0f));

	/* ----- Create the camera ----- */
	std::shared_ptr<Camera> main_camera = std::make_shared<Camera>();
	main_camera->SetAspectRatio(game_options.windowWidth / (float)game_options.windowHeight);
	main_camera->SetArmLength(5.0f);
	main_camera->SetArmAngleDegrees(glm::vec2(30.0f, 90.0f));
	spider_game.SetCurrentCamera(main_camera);

	/* ----- Render Loop ----- */
	// has GLFW been told to close?
	while (!glfwWindowShouldClose(spider_game.GetWindow())) {
		// Set the void color to dark green-blue (STATE-SETTING function)
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		// Clear the color & depth buffers (STATE-USING function)
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		basic_shader->Activate();
		// Practice with uniforms:
		GLfloat time = glfwGetTime();
		
		// Set the box's color
		GLfloat greenStrength = (sin(2.0 * time) / 2.0f) + 0.5f;
		// Set the uniform that's defined in the frag shader
		//   Note: the shader must be activated before calling this
		basic_shader->SetFloatUniform("greenStrength", greenStrength);
		
		// Render the boxes
		spider_game.RenderScene(basic_shader);
		cube1->SetRelativeRotationDegrees(glm::vec3(50.0 * time, 0.0f, 0.0f));
		cube1->PhysicsUpdate(glm::mat4(1.0f));
		cube1->Render(basic_shader);
		cube2->Render(basic_shader);

		// check and call events, swap buffers
		glfwSwapBuffers(spider_game.GetWindow()); // swap the color buffers
		glfwPollEvents(); // have any events been triggered (i.e. input?)
	}
	return 0;
}