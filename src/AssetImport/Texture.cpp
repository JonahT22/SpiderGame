#include "Texture.h"

#include <iostream>

#include "stb_image.h"

Texture::Texture(const std::string& filepath, const TextureType type,
                 const TextureOptions options) :
	type(type),
	textureID(0) {
	LoadFromFile(filepath, options);
}

Texture::~Texture() {
	// Deallocate the texture's GPU resources
	glDeleteTextures(1, &textureID);
}

void Texture::LoadFromFile(const std::string& filename, TextureOptions options) {
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
	GLenum external_format;
	switch (num_channels) {
	case 3:
		external_format = GL_RGB;
		break;
	case 4:
		external_format = GL_RGBA;
		break;
	default:
		std::cerr << "ERROR: Unhandled number of channels in loaded texture \"" << filename;
		std::cerr << "\": " << num_channels << std::endl;
		external_format = GL_RED;
	}
	if (data) {
		// Create the texture object
		// TODO remove?
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
		glTexImage2D(GL_TEXTURE_2D, 0, options.internalFormat, width, height, 0,
			external_format, GL_UNSIGNED_BYTE, data);
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

bool Texture::IsLoaded() const {
	return (textureID > 0);
}

Texture::TextureType Texture::GetType() const {
	return type;
}

void Texture::SetType(const TextureType new_type) {
	type = new_type;
}
