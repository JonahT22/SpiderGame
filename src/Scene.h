#pragma once
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include <utility>

class SceneObject;
class ShaderProgram;
class GameEngine;

/// 
/// Container class that manages all SceneObjects and Shaders in a level
///
class Scene {
public:
	Scene(std::weak_ptr<const GameEngine> engine);
	~Scene() = default;

	void UpdateScenePhysics();
	void RenderScene();
	void LoadHardcodedScene();

	/* ----- Getters ----- */

	/* ----- Setters ----- */
	// Instantiate every shader that will be used in this game
	void LoadShaders(const std::string filename);
	// Add a SceneObject to the Scene, to be drawn by the given shader
	void AddObjectToScene(const std::shared_ptr<SceneObject>& object,
	                      const std::string shader_name);
	// TODO: functions for removing objects from the scene

private:
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
};
