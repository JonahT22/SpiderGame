#version 330 core
/* ----- Uniforms ----- */

/* ----- In/Out ----- */
in vertexInfo {
	vec2 texCoord;
	vec3 normal;
};
// frag shader must ALWAYS output a vec4 for color
out vec4 fragColor;

void main() {
	fragColor = vec4((0.5 * normal) + vec3(0.5, 0.5, 0.5), 1.0);
}