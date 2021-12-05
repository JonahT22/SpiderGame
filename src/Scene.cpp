#include "Scene.h"

#include <iostream>
#include <fstream>

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

void Scene::LoadSceneFile(const std::string filename) {
	// yaml-cpp will silently crash if the file doesn't exist, so check for it first
	std::ifstream file(filename);
	if (!file) {
		std::cerr << "ERROR: Scene file \"" << filename << "\" not found!" << std::endl;
		abort();
	}
	file.close();

	YAML::Node full_scene = YAML::LoadFile(filename);

	/* ----- Load Shaders ----- */
	// Check that the scene file contains a list of shaders
	if (!full_scene["shaders"]) {
		// In the future, maybe make a 'default' shader
		std::cerr << "ERROR: No shaders listed in the scene file \"" << filename;
		std::cerr << "\"!" << std::endl;
	}

	// Get the Sequence with the data for every shader that will be used in the scene
	YAML::Node shaders = full_scene["shaders"];
	// Read every shader from the sequence
	for (size_t i = 0; i < shaders.size(); ++i) {
		std::string shader_name = YAML::GetMapVal<std::string>(shaders[i], "name");
		// ShaderToObjectList is typedef'd as a pair of a shader and a vector of object ptrs
		ShaderToObjectList new_shader_list;
		new_shader_list.first = std::make_shared<ShaderProgram>(shader_name);
		new_shader_list.first->Compile(YAML::GetMapVal<std::string>(shaders[i], "vert"),
		                               YAML::GetMapVal<std::string>(shaders[i], "frag"));
		allObjects.push_back(new_shader_list);
		// Keep a mapping between each shader's name and its index in the allObjects list
		shaderMap[shader_name] = allObjects.size() - 1;
	}

	/* ----- Load Scene Geometry ----- */
	// Check that the scene file contains a list of objects
	if (!full_scene["mesh_objects"]) {
		std::cerr << "ERROR: No mesh objects listed in the scene file \"" << filename;
		std::cerr << "\"!" << std::endl;
	}

	// Default texture options that each mesh will use
	TextureOptions tex_options{ GL_MIRRORED_REPEAT, GL_LINEAR_MIPMAP_LINEAR,
	                            GL_LINEAR, GL_RGB, GL_RGBA };

	YAML::Node meshes = full_scene["mesh_objects"];
	// Keep a (temporary) mapping from object names to their pointers, for setting
	//   parent-child relationships
	std::unordered_map<std::string, std::shared_ptr<SceneObject> > mesh_name_map;
	// Read every mesh from the sequence
	for (size_t i = 0; i < meshes.size(); ++i) {
		std::string mesh_name = YAML::GetMapVal<std::string>(meshes[i], "name");
		// Construct the mesh object, to be populated by the YAML file's parameters
		auto new_mesh = std::make_shared<Mesh>(engineRef, mesh_name);
		// Load the mesh file, or make the default cube if none is provided
		std::string mesh_filename = YAML::GetMapVal<std::string>(meshes[i], "meshfile");
		if (mesh_filename == "") {
			new_mesh->GenerateCubeMesh();
		}
		else {
			new_mesh->LoadMesh(mesh_filename);
		}
		// Load the mesh's texture
		// TODO: add support for 3-channel (i.e. jpg) textures
		new_mesh->LoadTexture(YAML::GetMapVal<std::string>(meshes[i], "texture"),
		                      tex_options);
		// Load the transform as a single map object
		new_mesh->SetRelativeTransform(YAML::GetMapVal<Transform>(meshes[i],
		                               "relative_transform"));
		// Set the mesh's parent object, if it has been created
		// TODO: this should probably be its own function
		std::string parent_name = YAML::GetMapVal<std::string>(meshes[i], "parent");
		bool is_root = (parent_name == "");
		if (is_root) {
			// TODO: add this object to the list of root objects
			std::cout << "found a root object: " << mesh_name << std::endl;
			if (mesh_name == "camera_cube") {
				// TODO: temporary hardcoding for camera
				new_mesh->AddChildObject(engineRef.lock()->GetMainCamera());
			}
		}
		else {
			if (mesh_name_map.count(parent_name) > 0) {
				std::shared_ptr<SceneObject> parent = mesh_name_map[parent_name];
				parent->AddChildObject(new_mesh);
			}
			else {
				std::cerr << "ERROR: SceneObject \"" << mesh_name << "\" attempted to";
				std::cerr << " parent itself under SceneObject \"" << parent_name;
				std::cerr <<  "\", but \"" << parent_name << "\" has not been read from";
				std::cerr << "the scene file!" << std::endl;
			}
		}


		// Add this mesh to its shader's list of meshes to draw each frame
		std::string shader_name = YAML::GetMapVal<std::string>(meshes[i], "shader");
		AddObjectToScene(new_mesh, shader_name, is_root);

		// Add this mesh to the mapping of mesh names
		mesh_name_map[mesh_name] = new_mesh;
	}

	// TODO: for now, manually update all of the root meshes (to propagate parent
	//   transforms to children)
	mesh_name_map["cube_center"]->PhysicsUpdate(glm::mat4(1.0f));
	mesh_name_map["camera_cube"]->PhysicsUpdate(glm::mat4(1.0f));
}

void Scene::AddObjectToScene(const std::shared_ptr<SceneObject>& object,
                             const std::string shader_name, const bool is_root) {
	// TODO: handle root objects
	
	if (shaderMap.count(shader_name) > 0) {
		allObjects.at(shaderMap[shader_name]).second.push_back(object);
	}
	else {
		std::cerr << "ERROR: Drawing object in scene, but no shader with name ";
		std::cerr << shader_name << " was loaded in the scene!" << std::endl;
	}
}
