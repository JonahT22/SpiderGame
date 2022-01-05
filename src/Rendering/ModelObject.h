#pragma once

#include <memory>
#include <string>
#include <vector>

#include "SceneObject.h"
class Model;
class Scene;
class Texture;

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

	// Set the texture that will override the model's loaded textures when drawing
	void SetTextureOverride(std::weak_ptr<Texture> tex_override);

	// Inherited from SceneObject
	virtual void BeginPlay() override;
	virtual void Render(const std::shared_ptr<ShaderProgram> shader) const override;

private:
	// Reference to a model that is managed by the scene. Multiple meshobjects can
	//   reference the same model
	std::weak_ptr<Model> model;

	// Optional texture override - if null, just use the textures that are loaded
	//   from the model's file
	std::weak_ptr<Texture> textureOverride;
};

