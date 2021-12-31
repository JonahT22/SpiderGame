#pragma once

#include <memory>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Texture.h"
class ShaderProgram;

///
/// Helper struct for storing mesh data (TODO: maybe move to separate file?)
/// 
struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoord;

	Vertex() : position(0.0f), normal(0.0f), texCoord(0.0f) {}
};

/// 
/// Loads & stores vertex data for a single static mesh
/// 
class StaticMesh {
public:
	StaticMesh(std::vector<Vertex>& vertices,
	           std::vector<GLuint>& indices,
	           std::vector<Texture>& textures);
	~StaticMesh();

	void Render(const std::shared_ptr<ShaderProgram> shader) const;

private:
	void SetupVertexArray();

	// Vertex Array Object - holds the mappings between buffers and attributes
	GLuint vertexArrayID = 0;

	// Vertex Buffer Objects - Stores raw vertex data
	// Note: all vertex data is currently stored in a single VBO. This is good when
	//   vertex data doesn't change, but if position data changes often (i.e. cloth sim),
	//   then keep position & normal in separate VBO from other data
	std::vector<Vertex> vertexBuffer;
	GLuint vertexBufferID = 0;

	// Element Buffer Object (optional) - Specifies which vertices to use for each triangle
	bool useEBO = false;
	std::vector<GLuint> elementBuffer;
	GLuint elementBufferID = 0;

	// Array of textures used by this mesh. Since textures are small objects (just
	//   an ID and type), each mesh can store full copies of its texture objects
	std::vector<Texture> textureList;
};

