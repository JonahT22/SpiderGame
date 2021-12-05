#pragma once
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include <utility>

#include <yaml-cpp/yaml.h>

class ShaderProgram;
class Skybox;
class SceneObject;
class GameEngine;
class Mesh;

/// 
/// Container class that manages all SceneObjects and Shaders in a level
///
class Scene {
public:
	Scene(std::weak_ptr<const GameEngine> engine);
	~Scene() = default;

	void UpdateScenePhysics();
	void RenderScene();

	/* ----- Getters ----- */

	/* ----- Setters ----- */
	// Instantiate every shader that will be used in this game
	void LoadSceneFile(const std::string& filename);
	// Add a SceneObject to the Scene, to be drawn by the given shader
	void AddObjectToScene(const std::shared_ptr<SceneObject>& object,
	                      const std::string shader_name);

private:
	// Loads parameters that ALL sceneobjects contain (i.e. transform, shader, etc.), then 
	//   handles object parenting and adds the object to the scene
	void LoadSceneObject(const YAML::Node object_node,
	                     const std::shared_ptr<SceneObject>& new_object,
	                     std::unordered_map<std::string, std::shared_ptr<SceneObject>>& object_name_map);

	// Weak reference to the GameEngine that manages this scene
	std::weak_ptr<const GameEngine> engineRef;
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

	// List of every root object in the scene (for calling physics updates)
	std::vector<std::weak_ptr<SceneObject> > rootObjects;

	// List of every physics object in the scene (for raycasting)
	// TODO

	// Skybox and its shader. The scene has exclusive control over the skybox
	std::unique_ptr<Skybox> skybox;
	std::unique_ptr<ShaderProgram> skyboxShader;
};
