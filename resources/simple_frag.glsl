#version 330 core

// Use same name and type as vert shader, OpenGL will link them together
in vec3 ourColor;
// frag shader must ALWAYS output a vec4 for color
out vec4 fragColor; 

void main() {
	fragColor = vec4(ourColor, 1.0);
}