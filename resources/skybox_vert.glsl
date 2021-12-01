// Opengl version 3.3, using the core profile
#version 330 core

/* ----- Attributes ----- */
layout (location = 0) in vec3 aPos;

/* ----- Uniforms ----- */
// View and Projection matrices (model matrix is not needed for skyboxes)
uniform mat4 V;
uniform mat4 P;

/* ----- Outputs ----- */
out vertexInfo {
	vec3 texCoord;
};


void main() {
	// For skyboxes, the vertex position is used to sample the texture
	texCoord = aPos;
	// Convert the vertex to normalized device coordinates (NDC)
	vec4 ndcPos = P * V * vec4(aPos, 1.0);
	// For the skybox to work, it needs to have the maximum possible depth so that it
	//   will render behind all other objects. In NDC, z is the depth of the object,
	//   and 1 is the maximum value it can have. So, set z = w so that after dividing by
	//   w, z will equal 1. 
	gl_Position = ndcPos.xyww;
}