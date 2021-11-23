#version 330 core
/* ----- Uniforms ----- */
uniform float greenStrength;
uniform sampler2D mainTexture;

/* ----- In/Out ----- */
in vertexInfo {
	vec3 vertColor;
	vec2 texCoord;
};
// frag shader must ALWAYS output a vec4 for color
out vec4 fragColor;

void main() {
	vec4 rainbowColor = vec4(vertColor.r, vertColor.g * greenStrength, vertColor.b, 1.0);
	// Combine the nearest vertex color & the texture
	//fragColor = mix(texture(mainTexture, texCoord), rainbowColor, 0.5);
	fragColor = texture(mainTexture, texCoord) * rainbowColor;
}