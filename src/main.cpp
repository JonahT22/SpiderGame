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
#include "GameInstance.h"
#include "Mesh.h"
// TODO: this shouldn't be necessary
#include "Texture.h"
// TODO: this shouldn't be necessary
#include "Camera.h"

int main() {
	// Initialize the game and main rendering window
	GameOptions game_options{ 800, 600, "Spider Game" };
	GameInstance spider_game(game_options);


	/* ----- Create scene geometry ----- */
	ShaderProgram basic_shader;
	basic_shader.Compile("resources/simple_vert.glsl", "resources/simple_frag.glsl");
	TextureOptions tex_options{ GL_MIRRORED_REPEAT, GL_LINEAR_MIPMAP_LINEAR,
								GL_LINEAR, GL_RGB, GL_RGBA };
	Mesh cube1;
	cube1.LoadMesh("asdf");
	cube1.LoadTexture("resources/awesomeface.png", tex_options);

	Mesh cube2;
	cube2.LoadMesh("asdf");
	cube2.LoadTexture("resources/awesomeface.png", tex_options);

	// Create the camera
	std::shared_ptr<Camera> mainCamera = std::make_shared<Camera>();
	mainCamera->SetAspectRatio(game_options.windowWidth / (float)game_options.windowHeight);
	mainCamera->SetArmLength(5.0f);
	// TODO: troubleshoot - I thought an angle of +90 would rotate camera CW, but it
	//   actually rotated CCW.
	mainCamera->SetArmAngleDegrees(glm::vec2(0.0f, 90.0f));
	spider_game.SetCurrentCamera(mainCamera);

	// ------------------------------------- RENDER LOOP ------------------------------------------------------
	// has GLFW been told to close? 
	while (!glfwWindowShouldClose(spider_game.GetWindow())) {
		// set the void color to dark green-blue (STATE-SETTING function)
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		// Clear the color & depth buffers (STATE-USING function)
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		basic_shader.Activate();
		// Practice with uniforms:
		GLfloat time = glfwGetTime();
		
		// Set the box's color
		GLfloat greenStrength = (sin(2.0 * time) / 2.0f) + 0.5f;
		// Set the uniform that's defined in the frag shader
		//   Note: the shader must be activated before calling this
		basic_shader.SetFloatUniform("greenStrength", greenStrength);
		
		// Render the boxes
		spider_game.RenderScene(basic_shader);
		// Set box 1 location to center of scene
		glm::mat4 M = glm::mat4(1.0f);
		basic_shader.SetMat4Uniform("M", M);
		cube1.Render();
		// Set box 2 location beside box 1
		M = glm::translate(M, glm::vec3(2.0f, 0.0f, 0.0f));
		basic_shader.SetMat4Uniform("M", M);
		cube2.Render();

		// check and call events, swap buffers
		glfwSwapBuffers(spider_game.GetWindow()); // swap the color buffers
		glfwPollEvents(); // have any events been triggered (i.e. input?)
	}
	return 0;
}