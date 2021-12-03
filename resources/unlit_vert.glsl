// Opengl version 3.3, using the core profile
#version 330 core

/* ----- Attributes ----- */
// Note: aPos is declared as a vec4 here, but it is sent as a vec3. OpenGL automatically
// adds a 'w' value of 1.0
layout (location = 0) in vec4 aPos;
layout (location = 2) in vec2 aTexCoord;

/* ----- Uniforms ----- */
// Model, view, and projection matrices
uniform mat4 Mvp;

/* ----- Outputs ----- */
out vertexInfo {
	vec2 texCoord;
};


void main() {
	// gl_Position is a built-in variable, and the final output of a vertex shader
	gl_Position = Mvp * aPos;
	texCoord = aTexCoord;
}