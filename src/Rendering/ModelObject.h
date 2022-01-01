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
	// Load the default model specified in the engine settings
	ModelObject(std::weak_ptr<GameEngine> engine, const std::string& name);
	// Load a specific model from a file path
	ModelObject(std::weak_ptr<GameEngine> engine, const std::string& name,
		const std::string& model_path);
	~ModelObject() = default;

	// Inherited from SceneObject
	virtual void BeginPlay() override;
	virtual void Render(const std::shared_ptr<ShaderProgram> shader) const override;

private:
	Model model;
};

