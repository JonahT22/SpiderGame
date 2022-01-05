#include <cassert>
#include <iostream>
#include <string>

#include "../AssetImport/Model.h"
#include "../GameEngine.h"
#include "ModelObject.h"
#include "Scene.h"
#include "ShaderProgram.h"

ModelObject::ModelObject(std::weak_ptr<GameEngine> engine, const std::string& name,
	const std::string& model_path) :
	SceneObject(engine, name) {
	std::string path;
	if (model_path == "") {
		// If no model is provided, load the default model from the engine
		path = engineRef.lock()->GetDefaultModelPath();
	}
	else {
		path = model_path;
	}
	// Get a reference to a model from the scene (the scene will create the model
	//   if a model from this filepath hasn't been loaded before)
	std::shared_ptr<Scene> scene_ref = engineRef.lock()->GetCurrentScene();
	model = scene_ref->GetModel(path);
}

void ModelObject::BeginPlay() {}

void ModelObject::Render(const std::shared_ptr<ShaderProgram> shader) const {
	// Call the parent's render function, which checks if the shader is bound and sends
	//   the model/invT matrices
	SceneObject::Render(shader);

	model.lock()->Render(shader);
}

