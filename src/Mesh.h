#pragma once

#include <vector>

#include <glad/glad.h>

#include "Texture.h"

///
/// Base class for any object that is made from a collection of renderable triangles
///
class Mesh {
public:
	Mesh();
	~Mesh();

	// Load mesh data into buffer vectors, but don't create the opengl buffer objects
	void LoadMesh(const char* filename);
	// Create the texture object that this mesh will use
	void LoadTexture(const char* filename, const TextureOptions& options);
	// Draw this mesh onto the screen
	void Render();

private:
	void SetupVertexArray();

	// Vertex Array Object - holds the mappings between buffers and attributes
	GLuint vertexArrayID;

	// Element Buffer Object - Specifies which vertices to use for each triangle
	std::vector<GLuint> elementBuffer;
	GLuint elementBufferID;

	// Vertex Buffer Objects - Stores raw vertex data
	// Note: each part of the mesh data is stored in a separate VBO. This is non-optimal,
	//   since they're no longer stored in a single contigous memory space. But it allows
	//   updating some parameters on-the-fly (i.e. position & normal) without having to
	//   re-send a single massive VBO. If no dynamic geometry is needed, it's better
	//   to keep everything in one VBO and just be careful when setting attributes.
	std::vector<GLfloat> positionBuffer;
	GLuint positionBufferID;
	// TODO: color probably shouldn't be stored on a point-by-point basis
	std::vector<GLfloat> colorBuffer;
	GLuint colorBufferID;
	std::vector<GLfloat> texCoordBuffer;
	GLuint texCoordBufferID;

	// Texture associated with this mesh
	// Note: for now, every object can have only 1 Texture, and textures cannot be
	//   shared by multiple meshes
	Texture texture;
};

