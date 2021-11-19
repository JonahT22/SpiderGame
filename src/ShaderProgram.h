#pragma once

#include <glad/glad.h>

/// 
/// Base class for all shaders, manages the compilation & 
/// lifetime of a shader program
/// 
class ShaderProgram {
public:
	ShaderProgram();
	~ShaderProgram() = default;
	
	// Compile & link the shader program
	void Init(const char* vert_path, const char* frag_path);
	void Activate();
	// TODO: is this necessary?
	void Deactivate();

	void Delete();

private:
	GLuint programID;
};

