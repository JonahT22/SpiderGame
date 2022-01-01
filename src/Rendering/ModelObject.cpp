#include <cassert>
#include <iostream>

#include "../GameEngine.h"
#include "ModelObject.h"
#include "ShaderProgram.h"

ModelObject::ModelObject(std::weak_ptr<GameEngine> engine, const std::string& name) :
	SceneObject(engine, name),
	model(engine.lock()->GetDefaultModelPath()) {}

ModelObject::ModelObject(std::weak_ptr<GameEngine> engine, const std::string& name,
	const std::string& model_path) :
	SceneObject(engine, name),
	model(model_path) {}

void ModelObject::BeginPlay() {}

void ModelObject::Render(const std::shared_ptr<ShaderProgram> shader) const {
	// Call the parent's render function, which checks if the shader is bound and sends
	//   the model/invT matrices
	SceneObject::Render(shader);

	model.Render(shader);
}

