// Opengl version 3.3, using the core profile
#version 330 core

/* ----- Attributes ----- */
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;


/* ----- Outputs ----- */
out vertexInfo {
	vec2 texCoord;
};


void main() {
	// gl_Position is a built-in variable, and the final output of a vertex shader
	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0); // vec4, with w = 1.0
	texCoord = aTexCoord;
}