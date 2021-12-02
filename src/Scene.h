#pragma once
#include <vector>
#include <memory>
#include <utility>

class SceneObject;
class ShaderProgram;

/// 
/// Container class that manages all SceneObjects and Shaders in a level
///
class Scene {
public:
	Scene() = default;
	~Scene() = default;

	void UpdateScenePhysics();
	void RenderScene();

	// (Temporary) Adder functions
	//void AddSceneObject(const std::shared_ptr<SceneObject>& object, )

private:
	// Master list of every object
	std::vector<std::shared_ptr<SceneObject> > allObjects;

	// List of every shader in the scene, and weak refs to the objects that they draw
	std::vector<std::pair<ShaderProgram, std::vector<std::weak_ptr<SceneObject> > > > shaderToObjects;

	// List of every root object in the scene (for calling physics updates)
	std::vector<std::weak_ptr<SceneObject> > rootObjects;

	// List of every physics object in the scene (for raycasting)
	// TODO
};
