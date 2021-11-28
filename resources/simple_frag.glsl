#version 330 core
/* ----- Uniforms ----- */
uniform float greenStrength;
// Samplers are bound to a specific texture by setting their (integer) texture unit
//   value when each mesh is drawn. If more textures are needed, they'll need to be
//   set up with their texture unit values in the shader program class
// By default, 'mainTexture' is bound to texture unit 0
uniform sampler2D mainTexture;

/* ----- In/Out ----- */
in vertexInfo {
	vec2 texCoord;
};
// frag shader must ALWAYS output a vec4 for color
out vec4 fragColor;

void main() {
	// TODO: temporary thing for learning uniforms
	vec4 rainbowColor = vec4(1.0, greenStrength, 1.0, 1.0);
	// Combine the nearest vertex color & the texture sample
	//fragColor = mix(texture(mainTexture, texCoord), rainbowColor, 0.5);
	fragColor = texture(mainTexture, texCoord) * rainbowColor;
}