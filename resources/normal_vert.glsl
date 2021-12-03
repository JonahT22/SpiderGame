// Opengl version 3.3, using the core profile
#version 330 core

/* ----- Attributes ----- */
// Note: aPos is declared as a vec4 here, but it is sent as a vec3. OpenGL automatically
// adds a 'w' value of 1.0
layout (location = 0) in vec4 aPos;
layout (location = 1) in vec3 aNor;
layout (location = 2) in vec2 aTexCoord;

/* ----- Uniforms ----- */
// Model, view, and projection matrices
uniform mat4 M;
uniform mat4 M_invT;
uniform mat4 V;
uniform mat4 V_invT;
uniform mat4 P;

/* ----- Outputs ----- */
out vertexInfo {
	vec2 texCoord;
	vec3 normal;
};


void main() {
	// gl_Position is a built-in variable, and the final output of a vertex shader
	gl_Position = P * V * M * aPos;
	texCoord = aTexCoord;
	// To transform normals into world space, need the inverse transpose of the modelview
	//   matrix: ((Mv)~)' = ((V*M)~)' = (V~*M~)' = (M~)'*(V~)'
	vec4 n = vec4(aNor, 0.0);
	normal = (M_invT * V_invT * n).xyz;
}