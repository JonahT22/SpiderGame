// Opengl version 3.3, using the core profile
#version 330 core

/* ----- Attributes ----- */
// Note: aPos is declared as a vec4 here, but it is sent as a vec3. OpenGL automatically
// adds a 'w' value of 1.0
layout (location = 0) in vec4 aPos;
layout (location = 1) in vec3 aNor;
layout (location = 2) in vec2 aTexCoord;

/* ----- Uniforms ----- */
// Modelview and projection matrices, where Mv = V * M
uniform mat4 Mv;
uniform mat4 Mv_invT;
uniform mat4 P;

/* ----- Outputs ----- */
out vertexInfo {
	vec4 position;
	vec3 normal;
};


void main() {
	// Position in eye coordinates, used for finding silhouettes
	position = Mv * aPos;
	gl_Position = P * position;
	vec4 n = vec4(aNor, 0.0);
	normal = (Mv_invT * n).xyz;
}