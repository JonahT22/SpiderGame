#pragma once

#include <memory>
#include <string>
#include <vector>

#include <assimp/scene.h>

#include "StaticMesh.h"
#include "Texture.h"
class ShaderProgram;

///
/// A model is a collection of StaticMeshes that are loaded from the same file
/// that are always drawn together.
/// Meshes in a model may use different textures, but always use the same shader.
/// Manages loading the mesh files from assimp and drawing the mesh objects
///
class Model {
public:
	Model(const std::string& filename);
	~Model() = default;

	void Render(const std::shared_ptr<ShaderProgram>& shader) const;

private:
	// Load the model into an assimp scene
	void LoadFromFile(const std::string& filename);
	void ProcessNode(aiNode* node, const aiScene* scene);
	StaticMesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
	void LoadTexturesFromMaterial(aiMaterial* material, aiTextureType ai_tex_type,
		Texture::TextureType custom_tex_type, std::vector<Texture>& tex_list);

	// Directory holding this model's file (not including the model's filename)
	std::string modelDir;
	std::vector<StaticMesh> meshList;
};