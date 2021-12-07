#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <yaml-cpp/yaml.h>

class Camera;
class GameEngine;
class Mesh;
class SceneObject;
class ShaderProgram;
class Skybox;

/// 
/// Container class that manages all SceneObjects and Shaders in a level
///
class Scene {
public:
	Scene(std::weak_ptr<GameEngine> engine);
	~Scene() = default;

	// Update the model matrices of every 'dirty' object in the scene
	void UpdateScenePhysics();
	// Iterate over each shader, rendering the objects that are drawn by it
	void RenderScene(const unsigned int frameDelayMs) const;
	// Instantiate every shader & SceneObject that will be used in this game
	void LoadSceneFile(const std::string& filename);

private:
	// Creates a mesh object and loads any mesh-specific parameters
	inline std::shared_ptr<Mesh> LoadMesh(const YAML::Node& mesh_node);
	// Creates a camera object and loads any camera-specific parameters
	inline std::shared_ptr<Camera> LoadCamera(const YAML::Node& camera_node,
	                                          bool is_first = false);
	// Loads parameters that ALL sceneobjects contain (i.e. transform, shader, etc.), then 
	//   handles object parenting and adds the object to the scene
	void LoadSceneObject(const YAML::Node& object_node,
	                     const std::shared_ptr<SceneObject>& new_object,
	                     std::unordered_map<std::string, std::shared_ptr<SceneObject>>& object_name_map);

	// Weak reference to the GameEngine that manages this scene
	std::weak_ptr<GameEngine> engineRef;
	// Hash map from shader names to their indices in allObjects
	// Note: All shaders must be loaded at the start of the game, and shaders cannot
	//   be created/deleted during runtime (if that is necessary, make sure to update
	//   this shadermap with the new indices of all shaders)
	std::unordered_map<std::string, size_t> shaderMap;

	// Pair containing a shader and a list of objects that it will be drawn with it
	// TODO: make these just regular shaderprograms, not shared_ptrs
	typedef std::pair<std::shared_ptr<ShaderProgram>,
	                  std::vector<std::shared_ptr<SceneObject> > > ShaderToObjectList;
	// List of every shader, and the objects that are drawn by them
	std::vector<ShaderToObjectList> allObjects;

	// List of every physics object in the scene (for raycasting)
	// TODO

	// Skybox and its shader. The scene has exclusive control over the skybox
	std::unique_ptr<Skybox> skybox;
	std::unique_ptr<ShaderProgram> skyboxShader;
};
