// Opengl version 3.3, using the core profile
#version 330 core

// The position variable has attribute position 0
layout (location = 0) in vec3 aPos;
// The color variable has attribute position 1
layout (location = 1) in vec3 aColor;
// Specify a color output to send to the fragment shader
// NOTE: some ppl say to use the 'varying' keyword instead of in/out. This is now deprecated
// see: https://stackoverflow.com/questions/34627576/why-did-glsl-change-varying-to-in-out
out vec3 ourColor;
	
void main() {
	// gl_Position is a built-in variable, and the final output of a vertex shader
	// For now, our vertex data is already in NDC, but usually we'd have to transform
	// from world -> normalized device coords so that they fall within OpenGL's visible region
	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0); // vec4, with w = 1.0
	// set ourColor to the input color we got from the vertex data
	ourColor = aColor;
}