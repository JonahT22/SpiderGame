// Opengl version 3.3, using the core profile
#version 330 core

/* ----- Attributes ----- */
// Note: aPos is declared as a vec4 here, but it is sent as a vec3. OpenGL automatically
// adds a 'w' value of 1.0
layout (location = 0) in vec4 aPos;

/* ----- Uniforms ----- */
// View and Projection matrix (model matrix is not needed for skyboxes). Vp = P * V
uniform mat4 Vp;

/* ----- Outputs ----- */
out vertexInfo {
	vec3 texCoord;
};


void main() {
	// For skyboxes, the vertex position is used to sample the texture
	texCoord = aPos.xyz;
	// Convert the vertex to normalized device coordinates (NDC)
	vec4 ndcPos = Vp * aPos;
	// For the skybox to work, it needs to have the maximum possible depth so that it
	//   will render behind all other objects. In NDC, z is the depth of the object,
	//   and 1 is the maximum value it can have. So, set z = w so that after dividing by
	//   w, z will equal 1. 
	gl_Position = ndcPos.xyww;
}