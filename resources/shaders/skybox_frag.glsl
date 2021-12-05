#version 330 core
/* ----- Uniforms ----- */
// Use a cube sampler, which uses 3D texture coordinates to sample a cubemap
uniform samplerCube cubeMap;

/* ----- In/Out ----- */
in vertexInfo {
	vec3 texCoord;
};
// frag shader must ALWAYS output a vec4 for color
out vec4 fragColor;

void main() {
	fragColor = texture(cubeMap, texCoord);
}