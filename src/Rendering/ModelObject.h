#pragma once

#include <memory>
#include <string>
#include <vector>

#include "SceneObject.h"
class Model;
class Scene;

///
/// Scene object containing a renderable static model
///
class ModelObject : public SceneObject {
public:
	// Load with optional model path. Note: if should_load_textures = false, then you
	//   must call SetTextureOverride before drawing the model
	ModelObject(std::weak_ptr<GameEngine> engine, const std::string& name,
		const std::string& model_path = "");
	~ModelObject() = default;

	// TODO: add function for setting an override texture

	// Inherited from SceneObject
	virtual void BeginPlay() override;
	virtual void Render(const std::shared_ptr<ShaderProgram> shader) const override;

private:
	// Reference to a model that is managed by the scene. Multiple meshobjects can
	//   reference the same model
	std::weak_ptr<Model> model;
};

