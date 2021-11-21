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

int main()
{
	// Initialize the game and main rendering window
	GameOptions game_options{ 800, 600, "Spider Game" };
	GameInstance spider_game(game_options);


	// ----------------------- HELLO TRIANGLE ------------------------
	ShaderProgram basic_shader;
	basic_shader.Init("resources/simple_vert.glsl", "resources/simple_frag.glsl");
	Mesh square_mesh;
	square_mesh.LoadMesh("asdf");

	//  uncomment this call to draw in wireframe polygons.
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
	// ------------------------------------- RENDER LOOP ------------------------------------------------------
	while (!glfwWindowShouldClose(spider_game.GetWindow())) // has GLFW been told to close?
	{
		// rendering commands
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // set the void color to dark green-blue (STATE-SETTING function)
		glClear(GL_COLOR_BUFFER_BIT); // only clear the color buffer, not depth or stencil buffer (STATE-USING function)
		
		// ---------------------------SHADERS--------------------------
		/* Practice with uniforms: 
		float timeValue = glfwGetTime();
		// interp a value from 0 to 1
		float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
		// Get the location of the uniform variable that we defined in the frag shader (-1 means not found)
		int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
		// set the shader (MUST be done before updating the uniform's value)
		glUseProgram(shaderProgram); 
		// set 'ourColor' to just green (alpha = 1). Note: uniform4f sets a value on the CURRENT shader, so have to set the shader first
		glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f); 
		// ^ in general, anytime a function is overloaded it will have a postfix for the type (OpenGL is built in C, so no overload support)
		// ex: glUniformfv = float vector/array (we could also use that here), glUniformui = unsigned int, etc...
		*/
		basic_shader.Activate();
		square_mesh.Render();
		// check and call events, swap buffers
		glfwSwapBuffers(spider_game.GetWindow()); // swap the color buffers
		glfwPollEvents(); // have any events been triggered (i.e. input?)
	}
	// TODO: eventually, this should be wrapped up in another class
	basic_shader.Delete();

	return 0;
}