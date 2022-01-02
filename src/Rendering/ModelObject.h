#pragma once

#include <string>
#include <vector>

#include "../AssetImport/Model.h"
#include "../AssetImport/Texture.h"
#include "SceneObject.h"

///
/// Scene object containing a renderable static model
///
class ModelObject : public SceneObject {
public:
	// Load with optional model path. Note: if should_load_textures = false, then you
	//   must call SetTextureOverride before drawing the model
	ModelObject(std::weak_ptr<GameEngine> engine, const std::string& name,
		const std::string& model_path = "", const Texture override_texture = Texture());
	~ModelObject() = default;

	// Inherited from SceneObject
	virtual void BeginPlay() override;
	virtual void Render(const std::shared_ptr<ShaderProgram> shader) const override;

private:
	Model model;
};

