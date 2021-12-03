// Opengl version 3.3, using the core profile
#version 330 core

/* ----- Attributes ----- */
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNor;
layout (location = 2) in vec2 aTexCoord;

/* ----- Uniforms ----- */
// Model, view, and projection matrices
uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

/* ----- Outputs ----- */
out vertexInfo {
	vec2 texCoord;
	vec3 normal;
};


void main() {
	// gl_Position is a built-in variable, and the final output of a vertex shader
	gl_Position = P * V * M * vec4(aPos, 1.0);
	texCoord = aTexCoord;
	// TODO: need to multiply by invT
	normal = aNor;
}