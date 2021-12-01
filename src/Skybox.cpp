#include "Skybox.h"

#include <iostream>

#include "stb_image.h"

#include "ShaderProgram.h"

// TODO: should I just automatically call loadcubemap and generateskyboxmesh from here?
Skybox::Skybox(const char* filenames[6]) {
	/* ----- Load the Cubemap from the texture files ----- */
	// Create the texture object
	glGenTextures(1, &cubeMapID);
	// Set this texture as the current cubemap texture
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapID);

	// Load the texture data from the image files
	int width, height, num_channels;
	for (size_t i = 0; i < 6; ++i) {
		unsigned char* data = stbi_load(filenames[i], &width, &height, &num_channels, 0);
		if (data) {
			// If data was loaded properly, create the texture object. GLenums for the
			//   different sides of the cubemap are defined sequentially in the following
			//   order: Right, Left, Top, Bottom, Front, Back
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0,
						 GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else {
			std::cerr << "ERROR: Failed to load cubemap texture \"" << filenames[i];
			std::cerr << "\"!" << std::endl;
		}
		stbi_image_free(data);
	}

	// Use linear filtering for minimization and magnification
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Use CLAMP_TO_EDGE to avoid artifacts near the seams of the cubemap
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	// Note: Cubemaps don't use 2D texture coordinates, they use 3D vectors to sample the
	//   texture. So, we also need to set behavior for the 3rd texture coordinate
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	/* ----- Generate a hardcoded mesh for the skybox ----- */
	GLfloat positions[] = {     
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};
	vertexBuffer.assign(std::begin(positions), std::end(positions));

	// Set up the VAO
	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);
	// Set up the VBO
	glGenBuffers(1, &vertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);

	// Send VBO data to GPU
	glBufferData(GL_ARRAY_BUFFER, vertexBuffer.size() * sizeof(GLfloat), 
	             &vertexBuffer[0], GL_STATIC_DRAW);
	// Create VBO vertex attributes for position only, no texcoords or normal required
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);

	// Unbind the VAO and VBO for safety
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

Skybox::~Skybox() {
	// Deallocate the vertex buffer and array
	glDeleteVertexArrays(1, &vertexArrayID);
	glDeleteBuffers(1, &vertexBufferID);
}

void Skybox::Render() {
	glBindVertexArray(vertexArrayID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapID);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}
