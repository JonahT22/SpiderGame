#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

/// 
/// Base class for all shaders, manages the compilation & 
/// lifetime of a shader program
/// 
class ShaderProgram {
public:
	ShaderProgram();
	~ShaderProgram();
	
	// Compile & link the shader program
	void Compile(const char* vert_path, const char* frag_path);
	void Activate();
	// TODO: is this necessary?
	void Deactivate();

	// Check if a provided uniform name exists
	GLint GetUniform(const GLchar* name);
	// Uniform setters
	void SetIntUniform(const GLchar* name, const GLint value);
	void SetFloatUniform(const GLchar* name, const GLfloat value);
	void SetMat4Uniform(const GLchar* name, const glm::mat4& matrix);

private:
	GLuint programID;
};

