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
	Model(const std::string& filename, std::weak_ptr<Scene> scene_ref);
	~Model() = default;

	// Render each mesh in the model, with an optional texture override
	void Render(const std::shared_ptr<ShaderProgram>& shader,
	            const std::weak_ptr<Texture> tex_override) const;

private:
	void ProcessNode(aiNode* node, const aiScene* scene,
		std::weak_ptr<Scene> scene_ref);
	void ProcessMesh(aiMesh* mesh, const aiScene* scene,
		std::weak_ptr<Scene> scene_ref);
	void LoadTexturesFromMaterial(aiMaterial* material,
	                              aiTextureType ai_tex_type,
	                              Texture::TextureType custom_tex_type,
	                              std::vector<std::weak_ptr<Texture> >& tex_list,
	                              std::weak_ptr<Scene> scene_ref);

	// Directory holding this model's file (not including the model's filename)
	std::string modelDir;
	std::vector<std::shared_ptr<StaticMesh> > meshList;
};