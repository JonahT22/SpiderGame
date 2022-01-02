#include <cassert>
#include <iostream>
#include <string>

#include "../GameEngine.h"
#include "ModelObject.h"
#include "ShaderProgram.h"

ModelObject::ModelObject(std::weak_ptr<GameEngine> engine, const std::string& name,
	const std::string& model_path, const Texture override_texture) :
	SceneObject(engine, name) {
	std::string path;
	if (model_path == "") {
		// If no model is provided, load the default model from the engine
		path = engineRef.lock()->GetDefaultModelPath();
	}
	else {
		path = model_path;
	}
	// If a valid override texture is provided, don't load textures from the model
	model.LoadFromFile(path, !override_texture.IsLoaded());
	// Instead, override them with the provided texture
	if (override_texture.IsLoaded()) {
		model.OverrideTextures(override_texture);
	}
}

void ModelObject::BeginPlay() {}

void ModelObject::Render(const std::shared_ptr<ShaderProgram> shader) const {
	// Call the parent's render function, which checks if the shader is bound and sends
	//   the model/invT matrices
	SceneObject::Render(shader);

	model.Render(shader);
}

