#pragma once

#include <glad/glad.h>

struct TextureOptions {
	// Sets GL_TEXTURE_WRAP_(S or T)
	GLenum wrapType = GL_MIRRORED_REPEAT;
	// Sets GL_TEXTURE_MIN_FILTER
	GLenum minFilter = GL_LINEAR_MIPMAP_LINEAR;
	// Sets GL_TEXTURE_MAX_FILTER
	GLenum maxFilter = GL_LINEAR;
	// Used in glTexImage2D
	GLenum internalFormat = GL_RGB;
	// Used in glTexImage2D
	GLenum externalFormat = GL_RGB;
};

/// 
/// Loads textures from files and stores their OpenGL object ID's
/// 
class Texture {
public:
	Texture();
	~Texture() = default;

	void LoadFromFile(const std::string& filename, const TextureOptions& options);
	// Binds this texture to the provided texture unit (default = 0)
	void Bind(GLuint texture_unit = 0) const;

	/* ----- Getters ----- */
	// Has this texture been assigned an image file?
	bool IsLoaded();

private:
	GLuint textureID;
};

