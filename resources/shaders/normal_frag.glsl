#version 330 core
/* ----- Uniforms ----- */

/* ----- In/Out ----- */
in vertexInfo {
	vec3 normal;
};
// frag shader must ALWAYS output a vec4 for color
out vec4 fragColor;

void main() {
	// Barycentric interpolation can make the normal vector length less than 1
	vec3 nVec = normalize(normal);
	fragColor = vec4((0.5 * nVec) + vec3(0.5, 0.5, 0.5), 1.0);
}