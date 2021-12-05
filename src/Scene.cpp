#include "Scene.h"

#include <iostream>
#include <yaml-cpp/yaml.h>

// TODO: is a full gameengine include necessary?
#include "GameEngine.h"
#include "ShaderProgram.h"
#include "SceneObject.h"
#include "Texture.h"
#include "Mesh.h"
#include "Camera.h"
#include "YAMLHelper.h"

Scene::Scene(std::weak_ptr<const GameEngine> engine) :
	engineRef(engine) {}

void Scene::UpdateScenePhysics() {

}

void Scene::RenderScene() {
	for (ShaderToObjectList& shader_to_object : allObjects) {
		// Iterate through every shader, and draw the objects associated with it
		std::shared_ptr<ShaderProgram> shader = shader_to_object.first;
		shader->Activate();
		// Try to set uniform variables in the shader, ignore them if they don't exist
		if (shader->GetUniform("P") != -1) {
			std::shared_ptr<Camera> main_camera = engineRef.lock()->GetMainCamera();
			shader->SetMat4Uniform("P", main_camera->GetProjectionMtx());
		}
		// TODO: more uniforms, i.e. time

		// Render every object associated with this shader
		for (std::shared_ptr<SceneObject>& object : shader_to_object.second) {
			object->Render(shader_to_object.first);
		}
	}
}

void Scene::LoadHardcodedScene() {
	/* ----- Create scene geometry ----- */
	TextureOptions tex_options{ GL_MIRRORED_REPEAT, GL_LINEAR_MIPMAP_LINEAR,
								GL_LINEAR, GL_RGB, GL_RGBA };
	auto cube1 = std::make_shared<Mesh>(engineRef);
	cube1->GenerateCubeMesh();
	cube1->LoadTexture("resources/textures/awesomeface.png", tex_options);
	cube1->SetRelativeLocation(glm::vec3(0.0f));

	auto cube2 = std::make_shared<Mesh>(engineRef);
	cube2->GenerateCubeMesh();
	cube2->LoadTexture("resources/textures/awesomeface.png", tex_options);
	cube2->SetRelativeLocation(glm::vec3(1.0f, 0.0f, 0.0f));
	cube2->SetRelativeScale(glm::vec3(0.5f, 0.5f, 0.5f));

	auto cube3 = std::make_shared<Mesh>(engineRef);
	cube3->GenerateCubeMesh();
	cube3->LoadTexture("resources/textures/awesomeface.png", tex_options);
	cube3->SetRelativeLocation(glm::vec3(0.0f, 0.0f, 1.0f));
	cube3->SetRelativeScale(glm::vec3(0.5f, 0.5f, 0.5f));

	cube1->AddChildObject(cube2);
	cube1->AddChildObject(cube3);
	cube1->PhysicsUpdate(glm::mat4(1.0f));

	auto cameraCube = std::make_shared<Mesh>(engineRef);
	cameraCube->GenerateCubeMesh();
	tex_options.externalFormat = GL_RGB;
	cameraCube->LoadTexture("resources/textures/wall.jpg", tex_options);
	cameraCube->SetRelativeLocation(glm::vec3(-2.0f, 0.0f, 0.0f));
	cameraCube->AddChildObject(engineRef.lock()->GetMainCamera());
	cameraCube->PhysicsUpdate(glm::mat4(1.0f));

	// Register all of the scene objects with the allObjects list
	AddObjectToScene(cube1, "unlit");
	AddObjectToScene(cube2, "normal");
	AddObjectToScene(cube3, "normal");
	AddObjectToScene(cameraCube, "unlit");
}

void Scene::LoadShaders(const std::string filename) {
	// TODO: load shader paths from a file. for now, just hardcode them
	allObjects.resize(2);

	ShaderToObjectList unlit_list;
	unlit_list.first = std::make_shared<ShaderProgram>();
	unlit_list.first->Compile("resources/shaders/unlit_vert.glsl",
	                          "resources/shaders/unlit_frag.glsl");

	ShaderToObjectList normal_list;
	normal_list.first = std::make_shared<ShaderProgram>();
	normal_list.first->Compile("resources/shaders/normal_vert.glsl",
	                           "resources/shaders/normal_frag.glsl");

	allObjects.at(0) = unlit_list;
	shaderMap["unlit"] = 0;
	allObjects.at(1) = normal_list;
	shaderMap["normal"] = 1;
}

void Scene::AddObjectToScene(const std::shared_ptr<SceneObject>& object,
                             const std::string shader_name) {
	if (shaderMap.count(shader_name) > 0) {
		allObjects.at(shaderMap[shader_name]).second.push_back(object);
	}
	else {
		std::cerr << "ERROR: Drawing object in scene, but no shader with name ";
		std::cerr << shader_name << " was loaded in the scene!" << std::endl;
	}
}
