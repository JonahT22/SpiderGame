#version 330 core
/* ----- Uniforms ----- */
uniform float greenStrength;

/* ----- In/Out ----- */
// Use same name and type as vert shader, OpenGL will link them together
in vec3 vertColor;
// frag shader must ALWAYS output a vec4 for color
out vec4 fragColor;

void main() {
	fragColor = vec4(vertColor.r, vertColor.g * greenStrength, vertColor.b, 1.0);
}