#pragma once

#include <vector>

#include <glad/glad.h>

#include "SceneObject.h"
#include "Texture.h"

///
/// Base class for any object that is made from a collection of renderable triangles
///
class Mesh : public SceneObject {
public:
	Mesh();
	~Mesh();

	// Load mesh data into buffer vectors, but don't create the opengl buffer objects
	void LoadMesh(const char* filename, const bool use_EBO = false);
	// Create the texture object that this mesh will use
	void LoadTexture(const char* filename, const TextureOptions& options);
	// Draw this mesh onto the screen
	virtual void Render(const std::shared_ptr<ShaderProgram> shader) override;

private:
	// Vertex Array Object - holds the mappings between buffers and attributes
	GLuint vertexArrayID;

	// Vertex Buffer Objects - Stores raw vertex data
	// Note: all vertex data is currently stored in a single VBO. This is good when
	//   vertex data doesn't change, but if position data changes often (i.e. cloth sim),
	//   then keep position/normal & other data in separate VBOs
	std::vector<GLfloat> vertexBuffer;
	GLuint vertexBufferID;

	// Element Buffer Object (optional) - Specifies which vertices to use for each triangle
	bool useEBO;
	std::vector<GLuint> elementBuffer;
	GLuint elementBufferID;

	// Texture associated with this mesh
	// Note: for now, every object can have only 1 Texture, and textures cannot be
	//   shared by multiple meshes
	Texture texture;
};

