#version 330 core
/* ----- Uniforms ----- */
uniform float time;

/* ----- In/Out ----- */
// frag shader must ALWAYS output a vec4 for color
out vec4 fragColor;

void main() {
	fragColor = vec4(0.5 * sin(time) + 0.7, 0.5 * cos(time) + 0.7, -0.5 * sin(time + 1) + 0.7, 1.0);
}