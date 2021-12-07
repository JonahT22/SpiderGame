#include "Scene.h"

#include <cassert>
#include <iostream>
#include <stdexcept>

#include "Camera.h"
#include "GameEngine.h"
#include "Mesh.h"
#include "SceneObject.h"
#include "ShaderProgram.h"
#include "Skybox.h"
#include "Texture.h"
#include "Window.h"
#include "YAMLHelper.h"

Scene::Scene(std::weak_ptr<GameEngine> engine) :
	engineRef(engine) {}

void Scene::UpdateScenePhysics() {
	for (auto& object_ref : rootObjects) {
		if (!object_ref.expired()) {
			object_ref.lock()->PhysicsUpdate();
		}
		else {
			// Eventually, the rootObjects list should updated whenever SceneObjects are
			//   removed from the scene. For now, just print an error message
			std::cerr << "ERROR: null reference to a SceneObject in rootObjects list";
			std::cerr << "while updating scene physics!" << std::endl;
		}
	}
}

void Scene::RenderScene() const {
	auto main_camera = engineRef.lock()->GetMainCamera();
	if (!main_camera) {
		std::cerr << "ERROR: No camera set in the game instance!" << std::endl;
		return;
	}
	/* ----- Draw every SceneObject ----- */
	for (const ShaderToObjectList& shader_to_object : allObjects) {
		// Iterate through every shader, and draw the objects associated with it
		std::shared_ptr<ShaderProgram> shader = shader_to_object.first;
		shader->Activate();
		// Try to set uniform variables in the shader, ignore them if they don't exist
		if (shader->GetUniform("P") != -1) {
			shader->SetMat4Uniform("P", main_camera->GetProjectionMtx());
		}
		// TODO: more uniforms, i.e. time

		// Render every object associated with this shader
		for (const std::shared_ptr<SceneObject>& object : shader_to_object.second) {
			object->Render(shader_to_object.first);
		}
	}

	/* ----- Draw the skybox ----- */
	// Send camera matrices to the shader
	skyboxShader->Activate();
	// Remove the translation factors from the view matrix by casting to a mat3
	// This works because the mat3->mat4 conversion places a 1 into unfilled
	//   diagonals, essentially setting the last column to (0, 0, 0, 1)T
	glm::mat4 view = glm::mat4(glm::mat3(main_camera->GetViewMtx()));
	skyboxShader->SetMat4Uniform("Vp", main_camera->GetProjectionMtx() * view, true);
	// Note: the skybox must be rendered AFTER all of the SceneObjects
	skybox->Render();
}

void Scene::LoadSceneFile(const std::string& filename) {
	// Catch any exceptions thrown by the YAML parser that aren't handled by custom
	//   error messages
	try {
	// Load the scene file
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

	/* ----- Load SceneObjects ----- */
	// Keep a (temporary) mapping from object names to their pointers, for setting
	//   parent-child relationships when loading SceneObjects
	std::unordered_map<std::string, std::shared_ptr<SceneObject> > object_name_map;

	// Load Meshes
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

	// Load Cameras
	assert(YAML::DoesMapHaveSequence(full_scene, "camera_objects"));
	YAML::Node cameras = full_scene["camera_objects"];
	// Read every camera from the sequence
	for (size_t i = 0; i < cameras.size(); ++i) {
		std::string camera_name = YAML::GetMapVal<std::string>(cameras[i], "name");
		auto new_camera = std::make_shared<Camera>(engineRef, camera_name);
		new_camera->SetAspectRatio(engineRef.lock()->GetWindow()->GetAspect());
		new_camera->SetFovDegrees(YAML::GetMapVal<float>(cameras[i], "fov_y"));
		new_camera->SetArmLength(YAML::GetMapVal<float>(cameras[i], "arm_length"));
		new_camera->SetArmAngleDegrees(YAML::GetMapVal<glm::vec2>(cameras[i], "arm_angle"));

		if (i == 0) {
			// By default, the GameEngine selects the first-listed camera as the main camera
			engineRef.lock()->SetCurrentCamera(new_camera);
		}

		// Once the camera-specific stuff is loaded, load the rest of the SceneObject properties
		LoadSceneObject(cameras[i], new_camera, object_name_map);
	}

	/* ----- Load the Skybox ----- */
	// Load the skybox shader
	YAML::Node skybox_node = full_scene["skybox"];
	// TODO: test if this crashes when skybox node is missing
	if (!skybox_node || !skybox_node.IsMap()) {
		std::cerr << "ERROR: No YAML Map with skybox properties found in scene file!";
		std::cerr << std::endl;
	}
	skyboxShader = std::make_unique<ShaderProgram>("skybox");
	skyboxShader->Compile(YAML::GetMapVal<std::string>(skybox_node, "vert"),
	                      YAML::GetMapVal<std::string>(skybox_node, "frag"));
	// Load the skybox images
	std::string cube_map_image_paths[6];
	const std::string side_names[6] = { "right", "left", "top", "bottom", "front", "back" };
	for (size_t i = 0; i < 6; ++i) {
		cube_map_image_paths[i] = YAML::GetMapVal<std::string>(skybox_node, side_names[i]);
	}
	skybox = std::make_unique<Skybox>(cube_map_image_paths);

	} // End try block
	catch (std::exception& e) {
		std::cerr << "ERROR::YAML parsing exception: " << e.what() << std::endl;
	}

	/* ----- Propagate parent-child transforms through the object hierarchy ----- */
	UpdateScenePhysics();
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
	if (parent_name != "") {
		if (object_name_map.count(parent_name) > 0) {
			// Attach this object to the parent with the given name
			std::weak_ptr<SceneObject> parent_object = object_name_map[parent_name];
			assert(!parent_object.expired());
			parent_object.lock()->AddChildObject(new_object);
		}
		else {
			std::cerr << "ERROR: SceneObject \"" << object_name << "\" attempted to";
			std::cerr << " parent itself under SceneObject \"" << parent_name;
			std::cerr <<  "\", but \"" << parent_name << "\" has not been read from";
			std::cerr << "the scene file!" << std::endl;
		}
	}
	else {
		// If no parent is provided, this object is a root object (a.k.a. it's parented
		//   to the world origin)
		rootObjects.emplace_back(new_object);
	}

	// Every object must be drawn by a shader, so get the shader's name
	std::string shader_name = YAML::GetMapVal<std::string>(object_node, "shader");
	// Add the object to the shader's list
	if (shaderMap.count(shader_name) > 0) {
		allObjects.at(shaderMap[shader_name]).second.push_back(new_object);
	}
	else {
		std::cerr << "ERROR: Drawing object in scene, but no shader with name ";
		std::cerr << shader_name << " was loaded in the scene!" << std::endl;
	}

	// Add this object to the mapping of object names, in case any other objects are
	//   parented to it
	object_name_map[object_name] = new_object;
}
