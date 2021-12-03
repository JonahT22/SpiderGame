#version 330 core
/* ----- Uniforms ----- */
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
	fragColor = texture(mainTexture, texCoord);
}