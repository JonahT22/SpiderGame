#pragma once

#include <memory>
#include <vector>
#include <iostream>

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
	Vertex(const glm::vec3& pos, const glm::vec3& nor, const glm::vec2& tex) :
		position(pos), normal(nor), texCoord(tex) {}
	void Print() {
		std::cout << "pos(" << position.x << ", " << position.y << ", " << position.z;
		std::cout << "), nor(" << normal.x << ", " << normal.y << ", " << normal.z;
		std::cout << "), tex(" << texCoord.x << ", " << texCoord.y << ")" << std::endl;
	}
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
	// Add a new texture to the texture list
	void AddTexture(const Texture& new_tex);
	// Remove every texture from this object
	void ClearTextures();

private:
	void SetupVertexArray();

	// Vertex Array Object - holds the mappings between buffers and attributes
	GLuint vertexArrayID;

	// Vertex Buffer Objects - Stores raw vertex data
	// Note: all vertex data is currently stored in a single VBO. This is good when
	//   vertex data doesn't change, but if position data changes often (i.e. cloth sim),
	//   then keep position & normal in separate VBO from other data
	std::vector<Vertex> vertexBuffer;
	GLuint vertexBufferID;

	// Element Buffer Object (optional) - Specifies which vertices to use for each triangle
	bool useEBO = false;
	std::vector<GLuint> elementBuffer;
	GLuint elementBufferID;

	// Array of textures used by this mesh. Since textures are small objects (just
	//   an ID and type), each mesh can store full copies of its texture objects
	std::vector<Texture> textureList;
};

