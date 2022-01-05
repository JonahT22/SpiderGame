#pragma once

#include <string>
#include <unordered_map>
#include <iostream>

#include <glad/glad.h>

struct TextureOptions {
	// Sets GL_TEXTURE_WRAP_(S or T)
	GLenum wrapType;
	// Sets GL_TEXTURE_MIN_FILTER
	GLenum minFilter;
	// Sets GL_TEXTURE_MAX_FILTER
	GLenum maxFilter;
	// Used in glTexImage2D
	// TODO: test with getting rid of this (i.e. if I read in a RGBA or RED image and
	//   represent it internally as RGBA or RED, will it cause issues?)
	GLenum internalFormat;
	// Default constructor
	TextureOptions() :
		wrapType(GL_REPEAT), minFilter(GL_LINEAR_MIPMAP_LINEAR),
		maxFilter(GL_LINEAR), internalFormat(GL_RGB) {}
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
	Texture(const std::string& filepath, const TextureType type,
	        const TextureOptions options);
	// Destructor deallocates this texture's GPU resources (Note: Do NOT make copies
	//   of Texture objects to avoid accidental deallocation)
	~Texture();

	// Binds this texture to the provided texture unit (default = 0)
	void Bind(GLuint texture_unit = 0) const;

	/* ----- Getters ----- */
	// Has this texture been correctly loaded from an image file?
	bool IsLoaded() const;
	TextureType GetType() const;

	/* ----- Setters ----- */
	void SetType(const TextureType new_type);

private:
	// Texture properties
	GLuint textureID;
	TextureType type;

	// Load and allocate this texture
	void LoadFromFile(const std::string& filename, TextureOptions options);
};

