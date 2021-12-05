#include "Texture.h"

#include <iostream>

#include "stb_image.h"

Texture::Texture() :
	textureID(0) 
{}

void Texture::LoadFromFile(const std::string& filename) {
	// TODO: eventually, this should be read from the scene configuration file. For now,
	//   just hardcode the texture options
	TextureOptions options{ GL_MIRRORED_REPEAT, GL_LINEAR_MIPMAP_LINEAR,
	                            GL_LINEAR, GL_RGB };

	// Create the texture object
	glGenTextures(1, &textureID);
	// Set this texture as the current texture, to be modified by further opengl calls
	glBindTexture(GL_TEXTURE_2D, textureID);

	// Set wrapping mode for s (u) and t (v) dimensions separately
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, options.wrapType);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, options.wrapType);
	// Set interpolation options
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, options.minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, options.maxFilter);

	// Load the texture data from the file
	stbi_set_flip_vertically_on_load(true);
	int width, height, num_channels;
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &num_channels, 0);
	if (data) {
		// Create the texture object
		// Arguments:
		//   GLenum target - which texture object in the current context is being created?
		//   GLint level - mipmap level to set (if you're setting them manually)
		//   GLint internalformat - how should openGL store the texture?
		//   GLsizei width, GLsizei height - texture's size
		//   GLint border - always 0 (legacy stuff)
		//   GLenum format - how is the texture represented in the file?
		//     Khronos group docs say it must match internalformat, but still works
		//     even if it doesn't
		//   GLenum type - what datatype is used to represent the image in the file?
		//   const void* pixels - the raw data
		// TODO: external format is currently hardcoded, but should be read from the number
		//   of channels in the image
		glTexImage2D(GL_TEXTURE_2D, 0, options.internalFormat, width, height, 0,
			GL_RGBA, GL_UNSIGNED_BYTE, data);
		// Let OpenGL create the other mipmaps automatically, instead of calling glTexImage2D for them
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cerr << "ERROR: Failed to load texture \"" << filename << "\".";
		std::cerr << std::endl;
	}
	stbi_image_free(data);
}

void Texture::Bind(GLuint texture_unit) const {
	// Check that the texture has been generated
	if (textureID <= 0) {
		std::cerr << "ERROR: Attempted to bind a texture that has not been loaded!";
		std::cerr << std::endl;
		return;
	}
	// When working with multiple textures, it's necessary to define the texture unit
	//   that each texture occupies when you bind it. Then, the shader can look in the
	//   corresponding texture unit's location when it accesses the sampler2D data
	glActiveTexture(GL_TEXTURE0 + texture_unit);
	glBindTexture(GL_TEXTURE_2D, textureID);
}

bool Texture::IsLoaded() {
	return (textureID > 0);
}
