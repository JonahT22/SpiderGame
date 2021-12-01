#pragma once

#include <vector>

#include <glad/glad.h>

class ShaderProgram;

/// 
/// Stores the cubemap texture and mesh data for a skybox
/// 
/// Note: even though the skybox shares properties with textures and meshes, it should
///   NOT share any inheritance with them. The skybox should be created and managed by
///   the GameEngine directly, and should not be moved or parented to other objects
/// 
class Skybox {
public:
	Skybox();
	~Skybox();

	// Load exactly 6 textures into a cubemap, supplied in the following order:
	//   Right, Left, Top, Bottom, Front, Back
	void LoadCubeMap(const char* filenames[6]);
	// Load a simple cube with only vertex data
	void GenerateSkyboxMesh();
	void Render();

private:
	// ID of the texure object that holds this skybox's cubemap
	GLuint cubeMapID;

	// Vertex Array Object - holds the mappings between buffers and attributes
	GLuint vertexArrayID;

	// VBO for this skybox's mesh data
	std::vector<GLfloat> vertexBuffer;
	GLuint vertexBufferID;
};

