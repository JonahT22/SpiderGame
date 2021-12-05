#pragma once

#include <vector>

#include <glad/glad.h>

class ShaderProgram;

/// 
/// Stores the cubemap texture and mesh data for a skybox
/// 
/// Note: even though the skybox shares properties with textures and meshes, it should
///   NOT share any inheritance with them. The skybox should be created and managed by
///   the GameEngine directly, and should not be managed by or parented to other objects
/// 
class Skybox {
public:
	// Supply exactly 6 texture files, given in the following order:
	//   Right, Left, Top, Bottom, Front, Back
	Skybox(const std::string filenames[6]);
	~Skybox();

	void Render();

private:
	// ID of the texure object that holds this skybox's cubemap
	GLuint cubeMapID = 0;
	// Vertex Array Object - holds the mappings between buffers and attributes
	GLuint vertexArrayID = 0;
	// VBO for this skybox's mesh data
	std::vector<GLfloat> vertexBuffer;
	GLuint vertexBufferID = 0;
};

