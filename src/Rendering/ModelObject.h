#pragma once

#include <string>
#include <vector>



#include "SceneObject.h"
#include "../AssetImport/Texture.h"

///
/// Scene object containing a renderable static model
///
class ModelObject : public SceneObject {
public:
	ModelObject(std::weak_ptr<GameEngine> engine, const std::string& name);
	~ModelObject() = default;

	// Inherited from SceneObject
	virtual void BeginPlay() override;

	// Load MeshObject data into buffer vectors
	void LoadMesh(const std::string& filename, const bool use_EBO = false);
	// Create a hardcoded cube for testing
	void GenerateCubeMesh(const float tex_mult_u = 1.0f, const float tex_mult_v = 1.0f);
	// Create the texture object that this mesh will use
	void LoadTexture(const std::string& filename);
	// Draw this mesh onto the screen
	virtual void Render(const std::shared_ptr<ShaderProgram> shader) const override;

private:

	// Texture associated with this mesh
	// Note: for now, every object can have only 1 Texture, and textures cannot be
	//   shared by multiple meshes
	Texture texture;
};

