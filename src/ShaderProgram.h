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
	void Compile(const char* vert_path, const char* frag_path);
	void Activate();
	// TODO: is this necessary?
	void Deactivate();
	// Completely deallocate this shader
	void Delete();

	// Check if a provided uniform name exists
	GLint GetUniform(const GLchar* name);
	// Uniform setters
	void SetIntUniform(const GLchar* name, const GLint value);
	void SetFloatUniform(const GLchar* name, const GLfloat value);

private:
	GLuint programID;
};

