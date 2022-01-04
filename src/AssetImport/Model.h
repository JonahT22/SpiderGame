#pragma once

#include <memory>
#include <string>
#include <vector>

#include <assimp/scene.h>

#include "StaticMesh.h"
#include "Texture.h"
class ShaderProgram;
class Scene;

///
/// A model is a collection of StaticMeshes and textures that are always drawn together.
/// Meshes in a model may use different textures, but always use the same shader.
/// The model class manages loading the mesh files from assimp and drawing the mesh objects.
///
class Model {
public:
	// Basic constructor - load from file
	Model() = default;
	~Model() = default;

	// Load the model into an assimp scene
	void LoadFromFile(const std::string& filename, bool load_textures = true);
	// Optional texture override - discard all textures in model when loading
	void OverrideTextures(const Texture& override_texture);
	void Render(const std::shared_ptr<ShaderProgram>& shader) const;

private:
	void ProcessNode(aiNode* node, const aiScene* scene, bool load_textures = true);
	void ProcessMesh(aiMesh* mesh, const aiScene* scene, bool load_textures = true);
	void LoadTexturesFromMaterial(aiMaterial* material, aiTextureType ai_tex_type,
		Texture::TextureType custom_tex_type, std::vector<Texture>& tex_list);

	// Directory holding this model's file (not including the model's filename)
	std::string modelDir;
	std::vector<std::shared_ptr<StaticMesh> > meshList;
	// Reference to the scene for managing textures
	std::weak_ptr<Scene> sceneRef;
};