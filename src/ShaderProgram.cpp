#include "ShaderProgram.h"

#include <fstream>
#include <iostream>
#include <string>

ShaderProgram::ShaderProgram() :
	programID(0)
{}

void ShaderProgram::Compile(const char* vert_path, const char* frag_path) {
	/* ----- Get the shader source code ----- */
	// Open the shader files
	std::ifstream vert_file(vert_path);
	if (!vert_file.good()) {
		std::cerr << "Error reading vertex shader file: " << vert_path << std::endl;
	}
	std::ifstream frag_file(frag_path);
	if (!frag_file.good()) {
		std::cerr << "Error reading fragment shader file: " << frag_path << std::endl;
	}
	// Construct a string for the files by reading directly from the buffer with an iterator.
	// Note: istreambuf_iterator's default constructor is a special end-of-stream iterator
	std::string vert_string((std::istreambuf_iterator<char>(vert_file)),
	                        (std::istreambuf_iterator<char>()));
	std::string frag_string((std::istreambuf_iterator<char>(frag_file)),
	                        (std::istreambuf_iterator<char>()));

	const char* vert_data = vert_string.c_str();
	const char* frag_data = frag_string.c_str();

	/* ----- Create and compile the shaders ----- */
	// Create the shader objects in the opengl context
	GLuint vert_shader = glCreateShader(GL_VERTEX_SHADER);
	GLuint frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
	// Attach the shader source code to the objects
	glShaderSource(vert_shader, 1, &vert_data, NULL);
	glShaderSource(frag_shader, 1, &frag_data, NULL);
	// Create buffers for getting shader compilation status
	int success;
	char infoLog[512];
	// Compile vertex shader
	glCompileShader(vert_shader);
	glGetShaderiv(vert_shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vert_shader, 512, NULL, infoLog);
		std::cerr << "Error during compilation of vertex shader!\n" << infoLog << std::endl;
	}
	// Compile frag shader
	glCompileShader(frag_shader);
	glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(frag_shader, 512, NULL, infoLog);
		std::cerr << "Error during compilation of fragment shader!\n" << infoLog << std::endl;
	}

	/* ----- Link the shaders into a program ----- */
	// Note: when linking, it links the OUTPUTS of each shader to the INPUTS of the NEXT shader
	programID = glCreateProgram();
	// Attach the shaders to the program
	glAttachShader(programID, vert_shader);
	glAttachShader(programID, frag_shader);
	// Link the two attached shaders together
	glLinkProgram(programID);
	// Handle error checking, just like before
	glGetProgramiv(programID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(programID, 512, NULL, infoLog);
		std::cerr << "Error linking shader program!\n" << infoLog << std::endl;
	}
	// Since the shaders are now linked to the program, we don't need them anymore
	glDeleteShader(vert_shader);
	glDeleteShader(frag_shader);
}

void ShaderProgram::Activate() {
	glUseProgram(programID);
}

void ShaderProgram::Deactivate() {
	glUseProgram(0);
}

void ShaderProgram::Delete() {
	glDeleteProgram(programID);
}

GLint ShaderProgram::GetUniform(const GLchar* name) {
	// TODO: probably also need to check that this shader is active
	GLint location = glGetUniformLocation(programID, name);
	if (location >= 0) {
		return location;
	}
	else {
		std::cerr << "ERROR: Uniform \"" << name << "\" does not exist in shader!";
		std::cerr << std::endl;
		abort();
	}
}

void ShaderProgram::SetIntUniform(const GLchar* name, const GLint value) {
	GLint location = GetUniform(name);
	glUniform1i(location, value);
}

void ShaderProgram::SetFloatUniform(const GLchar* name, const GLfloat value) {
	GLint location = GetUniform(name);
	glUniform1f(location, value);
}
