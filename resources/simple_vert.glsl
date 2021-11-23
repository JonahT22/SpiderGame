// Opengl version 3.3, using the core profile
#version 330 core

/* ----- Attributes ----- */
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

/* ----- Outputs ----- */
out vertexInfo {
	vec3 vertColor;
	vec2 texCoord;
};


void main() {
	// gl_Position is a built-in variable, and the final output of a vertex shader
	// For now, our vertex data is already in NDC, but usually we'd have to transform
	// from world -> normalized device coords so that they fall within OpenGL's visible region
	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0); // vec4, with w = 1.0
	// set ourColor to the input color we got from the vertex data
	vertColor = aColor;
	texCoord = aTexCoord;
}