#pragma once

#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>

/// 
/// Base class for all shaders, manages the compilation & 
/// lifetime of a shader program
/// 
class ShaderProgram {
public:
	ShaderProgram() = default;
	ShaderProgram(const std::string& name);
	~ShaderProgram();

	// Compile & link the shader program
	void Compile(const std::string& vert_path, const std::string& frag_path);
	// Set this shader as OpenGL's current program
	void Activate();
	// Note: deactivating shaders is optional
	//   (See https://stackoverflow.com/questions/13546461/what-does-gluseprogram0-do)
	void Deactivate();

	/* ----- Getters ----- */
	// Return index of uniform if it exists, -1 if it doesn't
	GLint GetUniform(const GLchar* name, bool verbose = false) const;
	bool IsShaderActive() const;
	const std::string GetShaderName() const;

	/* ----- Setters ----- */
	void SetIntUniform(const GLchar* name, const GLint value, bool verbose = false);
	void SetFloatUniform(const GLchar* name, const GLfloat value, bool verbose = false);
	void SetMat4Uniform(const GLchar* name, const glm::mat4& matrix, bool verbose = false);

private:
	const std::string shaderName = "unnamed_shader";
	GLuint programID = 0;
};

