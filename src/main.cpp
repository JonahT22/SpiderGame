#include <iostream>
#include <string>

// TODO: I shouldn't need to include glad and glfw here, should be abstracted
// behind helper classes
// Always include glad before glfw
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"

#include "ShaderProgram.h"
#include "GameInstance.h"
#include "Mesh.h"

int main() {
	// Initialize the game and main rendering window
	GameOptions game_options{ 800, 600, "Spider Game" };
	GameInstance spider_game(game_options);


	// ----------------------- HELLO TRIANGLE ------------------------
	ShaderProgram basic_shader;
	basic_shader.Compile("resources/simple_vert.glsl", "resources/simple_frag.glsl");
	Mesh square_mesh;
	square_mesh.LoadMesh("asdf");

	/* ----- Create a texture ----- */
	GLuint tex_id;
	glGenTextures(1, &tex_id);
	// Set this texture as the current texture, to be modified by further opengl calls
	glBindTexture(GL_TEXTURE_2D, tex_id);

	/* ----- Set texture processing options for the currently-bound texture ----- */
	// Set wrapping mode for s (u) and t (v) dimensions separately
	// Options: 
	//   GL_REPEAT - default, just tiles texture
	//   GL_MIRRORED_REPEAT - flips textures so edges line up
	//   GL_CLAMP_TO_EDGE - clamps texcoords from 0-1 so edges stretch to border
	//   GL_CLAMP_TO_BORDER - replaces texcoords outside [0, 1] with a color
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	// Set the way that OpenGL will handle mipmaps when sampling textures
	//   Mipmaps only really matter for minimizing
	//     GL_LINEAR_MIPMAP_LINEAR linearly interpolates btwn 2 nearest mipmaps,
	//     then linearly interpolates that result
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	// Just use regular linear interp for magnifying - doesn't use mipmaps at all
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	/* ----- Load the texture data ----- */
	int width, height, num_channels;
	const char* tex_filename = "resources/wall.jpg";
	unsigned char* data = stbi_load(tex_filename, &width, &height, &num_channels, 0);
	if (data) {
		// Arguments:
		//   GLenum target - which texture object in the current context is being created?
		//   GLint level - mipmap level to set (if you're setting them manually)
		//   GLint internalformat - how should openGL store the texture?
		//   GLsizei width, GLsizei height - texture's size
		//   GLint border - always 0 (legacy stuff)
		//   GLenum format - how is the texture represented in the file?
		//   GLenum type - what datatype is used to represent the image in the file?
		//   const void* pixels - the raw data
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		// Let OpenGL create the other mipmaps automatically, instead of calling glTexImage2D for them
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cerr << "ERROR: Failed to load texture \"" << tex_filename << "\".";
		std::cerr << std::endl;
	}
	stbi_image_free(data);


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
		// When working with multiple textures, you should define the current one's location
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex_id);
		square_mesh.Render();
		// check and call events, swap buffers
		glfwSwapBuffers(spider_game.GetWindow()); // swap the color buffers
		glfwPollEvents(); // have any events been triggered (i.e. input?)
	}
	return 0;
}