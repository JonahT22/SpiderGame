#pragma once

#include <string>
#include <vector>

#include <glad/glad.h>


///
/// Base class for any object that is made from a collection of renderable triangles
///
class Mesh {
public:
	Mesh();
	~Mesh();

	// Load mesh data into buffer vectors, but don't create the opengl buffer objects
	void LoadMesh(const std::string& filename);
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
	std::vector<GLfloat> positionBuffer;
	GLuint positionBufferID;
	// TODO: color probably shouldn't be stored on a point-by-point basis
	std::vector<GLfloat> colorBuffer;
	GLuint colorBufferID;
};

