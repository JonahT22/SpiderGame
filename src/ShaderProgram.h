#pragma once

// Get opengl function headers
#include <glad/glad.h>

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

