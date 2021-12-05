#include "Scene.h"

#include <iostream>
#include <fstream>
#include <cassert>

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

void Scene::LoadSceneFile(const std::string filename) {
	// yaml-cpp will silently crash if the file doesn't exist, so check for it first
	std::ifstream file(filename);
	if (!file) {
		std::cerr << "ERROR: Scene file \"" << filename << "\" not found!" << std::endl;
		assert(0);
	}
	file.close();

	YAML::Node full_scene = YAML::LoadFile(filename);

	/* ----- Load Shaders ----- */
	assert(YAML::DoesMapHaveSequence(full_scene, "shaders"));
	// Get the sequence of shaders (YAML::Sequence is similar to a std::vector)
	YAML::Node shaders = full_scene["shaders"];
	for (size_t i = 0; i < shaders.size(); ++i) {
		std::string shader_name = YAML::GetMapVal<std::string>(shaders[i], "name");
		// ShaderToObjectList is typedef'd as a std::pair with a shader and
		//    a vector of object ptrs
		ShaderToObjectList new_shader_list;
		new_shader_list.first = std::make_shared<ShaderProgram>(shader_name);
		// Load and compile the shader program from the filepaths provided in the YAML file
		new_shader_list.first->Compile(YAML::GetMapVal<std::string>(shaders[i], "vert"),
										YAML::GetMapVal<std::string>(shaders[i], "frag"));
		// Store this shader list, with a blank vector of object ptrs to be populated later
		allObjects.push_back(new_shader_list);
		// Keep a mapping between each shader's name and its index in the allObjects list
		shaderMap[shader_name] = allObjects.size() - 1;
	}

	// Keep a (temporary) mapping from object names to their pointers, for setting
	//   parent-child relationships when loading SceneObjects
	std::unordered_map<std::string, std::shared_ptr<SceneObject> > object_name_map;

	/* ----- Load Scene Geometry ----- */
	assert(YAML::DoesMapHaveSequence(full_scene, "mesh_objects"));
	YAML::Node meshes = full_scene["mesh_objects"];
	// Read every mesh from the sequence
	for (size_t i = 0; i < meshes.size(); ++i) {
		std::string mesh_name = YAML::GetMapVal<std::string>(meshes[i], "name");
		auto new_mesh = std::make_shared<Mesh>(engineRef, mesh_name);
		// Load the mesh file, or make the default cube if none is provided
		std::string mesh_filename = YAML::GetMapVal<std::string>(meshes[i], "meshfile");
		if (mesh_filename == "") {
			new_mesh->GenerateCubeMesh();
		}
		else {
			new_mesh->LoadMesh(mesh_filename);
		}
		new_mesh->LoadTexture(YAML::GetMapVal<std::string>(meshes[i], "texture"));

		// Once the mesh-specific stuff is loaded, load the rest of the SceneObject properties
		LoadSceneObject(meshes[i], new_mesh, object_name_map);
	}

	// TODO: for now, manually update all of the root meshes (to propagate parent
	//   transforms to children)
	object_name_map["cube_center"]->PhysicsUpdate(glm::mat4(1.0f));
	object_name_map["camera_cube"]->PhysicsUpdate(glm::mat4(1.0f));
	// TODO later, do this instead
	//UpdateScenePhysics();
}

void Scene::AddObjectToScene(const std::shared_ptr<SceneObject>& object,
                             const std::weak_ptr<SceneObject>& parent,
                             const std::string shader_name) {
	// TODO: I shouldn't need to pass in the parent here, just query it from the object
	// TODO: temporary debugging stuff
	std::string object_name = object->GetName();
	if (parent.expired()) {
		std::cout << "found a root object: " << object_name << std::endl;
		// TODO: temporary hardcoding for camera
		if (object_name == "camera_cube") {
			object->AddChildObject(engineRef.lock()->GetMainCamera());
		}
	}
	else {
		parent.lock()->AddChildObject(object);
	}

	if (shaderMap.count(shader_name) > 0) {
		allObjects.at(shaderMap[shader_name]).second.push_back(object);
	}
	else {
		std::cerr << "ERROR: Drawing object in scene, but no shader with name ";
		std::cerr << shader_name << " was loaded in the scene!" << std::endl;
	}
}

void Scene::LoadSceneObject(const YAML::Node object_node,
                            const std::shared_ptr<SceneObject>& new_object,
                            std::unordered_map<std::string, std::shared_ptr<SceneObject>>& object_name_map) {
	// Load the transform as a single map object
	new_object->SetRelativeTransform(YAML::GetMapVal<Transform>(object_node,
		                            "relative_transform"));
	
	// Set the parent object, if it has been created
	const std::string object_name = new_object->GetName();
	const std::string parent_name = YAML::GetMapVal<std::string>(object_node, "parent");
	// By default, the parent ref is null. If the object's 'parent' field matches another
	//   object that has already been loaded, then change the parent ref to that object
	std::weak_ptr<SceneObject> parent_object;
	if (parent_name != "") {
		if (object_name_map.count(parent_name) > 0) {
			parent_object = object_name_map[parent_name];
		}
		else {
			std::cerr << "ERROR: SceneObject \"" << object_name << "\" attempted to";
			std::cerr << " parent itself under SceneObject \"" << parent_name;
			std::cerr <<  "\", but \"" << parent_name << "\" has not been read from";
			std::cerr << "the scene file!" << std::endl;
		}
	}

	// Every object must be drawn by a shader, so get the shader's name
	std::string shader_name = YAML::GetMapVal<std::string>(object_node, "shader");

	// Add the object to the shader's list, and set its parent-child relationships
	AddObjectToScene(new_object, parent_object, shader_name);

	// Add this mesh to the mapping of object names
	object_name_map[object_name] = new_object;
}
