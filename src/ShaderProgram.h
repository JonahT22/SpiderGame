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
	
	/* ----- Getters ----- */
	// Return index of uniform if it exists, -1 if it doesn't
	GLint GetUniform(const GLchar* name, bool verbose = false);
	bool IsShaderActive();

	/* ----- Setters ----- */
	void SetIntUniform(const GLchar* name, const GLint value, bool verbose = false);
	void SetFloatUniform(const GLchar* name, const GLfloat value, bool verbose = false);
	void SetMat4Uniform(const GLchar* name, const glm::mat4& matrix, bool verbose = false);

	/* ----- Other Functions ----- */
	// Compile & link the shader program
	void Compile(const char* vert_path, const char* frag_path);
	void Activate();
	// Note: deactivating shaders is optional
	//   (See https://stackoverflow.com/questions/13546461/what-does-gluseprogram0-do)
	void Deactivate();

private:
	GLuint programID;
};

