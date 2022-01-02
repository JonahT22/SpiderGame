#pragma once

#include <string>
#include <unordered_map>
#include <iostream>

#include <glad/glad.h>

struct TextureOptions {
	// Sets GL_TEXTURE_WRAP_(S or T)
	GLenum wrapType = GL_REPEAT;
	// Sets GL_TEXTURE_MIN_FILTER
	GLenum minFilter = GL_LINEAR_MIPMAP_LINEAR;
	// Sets GL_TEXTURE_MAX_FILTER
	GLenum maxFilter = GL_LINEAR;
	// Used in glTexImage2D
	GLenum internalFormat = GL_RGB;
};

/// 
/// Loads textures from files and stores their OpenGL object ID's
/// 
class Texture {
public:
	/* ----- Enums & static functions ----- */
	// List of texture types that are supported by shaders
	enum class TextureType : GLuint {
		DIFFUSE,
		SPECULAR,
		NORMAL,
		// Last entry MUST be ENUM_END, used for iterating over texture types
		ENUM_END
	};
	// Mapping from texture types to strings with their names in shader programs
	inline static std::string TypeToString(const TextureType type) {
		switch (type) {
		case TextureType::DIFFUSE:
			return "Diffuse";
			break;
		case TextureType::SPECULAR:
			return "Specular";
			break;
		case TextureType::NORMAL:
			return "Normal";
			break;
		default:
			std::cerr << "ERROR: Unhandled texture type in Texture::TypeToString!" << std::endl;
		}
		return "undefined_texture_type";
	}

	/* ----- Class Definition ----- */
	// Default (invalid) constructor
	Texture();
	// Proper constructor from file
	Texture(const std::string& filepath, const TextureType type);
	// Note: Do NOT call glDeleteTextures when destructed. Multiple Texture objects can
	//   reference the same glTexture (i.e. one of them actually allocates the texture,
	//   and the others just store a copy of the textureID)
	~Texture() = default;

	// Binds this texture to the provided texture unit (default = 0)
	// TODO: is Bind() needed when the staticmesh handles all the texture binding stuff?
	void Bind(GLuint texture_unit = 0) const;

	/* ----- Getters ----- */
	// Has this texture been correctly loaded from an image file?
	bool IsLoaded() const;
	TextureType GetType() const;

private:
	// Texture properties
	GLuint textureID;
	TextureType type;

	void LoadFromFile(const std::string& filename);
};

