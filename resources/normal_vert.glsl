// Opengl version 3.3, using the core profile
#version 330 core

/* ----- Attributes ----- */
// Note: aPos is declared as a vec4 here, but it is sent as a vec3. OpenGL automatically
// adds a 'w' value of 1.0
layout (location = 0) in vec4 aPos;
layout (location = 1) in vec3 aNor;

/* ----- Uniforms ----- */
// Model, view, and projection matrices
uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

/* ----- Outputs ----- */
out vertexInfo {
	vec3 normal;
};


void main() {
	// gl_Position is a built-in variable, and the final output of a vertex shader
	gl_Position = P * V * M * aPos;
	normal = aNor;
}