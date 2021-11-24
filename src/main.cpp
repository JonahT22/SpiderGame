#include <iostream>
#include <string>

// TODO: I shouldn't need to include glad and glfw here, should be abstracted
// behind helper classes
// Always include glad before glfw
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "ShaderProgram.h"
#include "GameInstance.h"
#include "Mesh.h"
// TODO: this shouldn't be necessary
#include "Texture.h"

int main() {
	// Initialize the game and main rendering window
	GameOptions game_options{ 800, 600, "Spider Game" };
	GameInstance spider_game(game_options);


	/* ----- Create scene geometry ----- */
	ShaderProgram basic_shader;
	basic_shader.Compile("resources/simple_vert.glsl", "resources/simple_frag.glsl");
	TextureOptions tex_options{ GL_MIRRORED_REPEAT, GL_LINEAR_MIPMAP_LINEAR,
								GL_LINEAR, GL_RGB, GL_RGB };
	Mesh square_mesh;
	square_mesh.LoadMesh("asdf");
	square_mesh.LoadTexture("resources/wall.jpg", tex_options);
	tex_options.externalFormat = GL_RGBA;
	Mesh square_mesh2;
	square_mesh2.LoadMesh("asdf");
	square_mesh2.LoadTexture("resources/awesomeface.png", tex_options);


	//  uncomment this call to draw in wireframe polygons.
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// ------------------------------------- RENDER LOOP ------------------------------------------------------
	// has GLFW been told to close? 
	while (!glfwWindowShouldClose(spider_game.GetWindow())) {
		// rendering commands
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // set the void color to dark green-blue (STATE-SETTING function)
		glClear(GL_COLOR_BUFFER_BIT); // only clear the color buffer, not depth or stencil buffer (STATE-USING function)

		basic_shader.Activate();
		// Practice with uniforms:
		GLfloat time = glfwGetTime();
		// interp a value from 0 to 1
		GLfloat greenStrength = (sin(2.0 * time) / 2.0f) + 0.5f;
		// Set the uniform that's defined in the frag shader
		//   Note: the shader must be activated before calling this
		basic_shader.SetFloatUniform("greenStrength", greenStrength);
		if ((int)floor(time) % 2 == 0) {
			square_mesh.Render();
		}
		else {
			square_mesh2.Render();
		}
		// check and call events, swap buffers
		glfwSwapBuffers(spider_game.GetWindow()); // swap the color buffers
		glfwPollEvents(); // have any events been triggered (i.e. input?)
	}
	return 0;
}