#pragma once

#include <memory>

#include <glm/glm.hpp>

class Window;
class Camera;
class ShaderProgram;
class Skybox;
struct GLFWwindow;
class Scene;

struct GameOptions {
	const int windowWidth = 256;
	const int windowHeight = 256;
	const char* windowName = "New Game Window";
};

///
/// Handles rendering frames, calling physics updates, managing the
///   main rendering window and currently-active scene, and sending player
///   inputs to the correct objects
///
/// Note: GameEngine must inherit from enable_shared_from_this so that it can populate
///   newly-created SceneObjects with weak references to this GameEngine
class GameEngine : public std::enable_shared_from_this<GameEngine> {
public:
	GameEngine(const GameOptions options);
	~GameEngine();
	
	// Getters
	GLFWwindow* GetWindow() const;
	std::shared_ptr<Camera> GetMainCamera() const;

	// Setters
	void SetCurrentCamera(const std::shared_ptr<Camera> new_camera);

	// Input events (from the mainWindow)
	void InputMoveCamera(glm::vec2 motion) const;
	void UpdateCameraAspect(const float new_aspect) const;

	// Other Functions
	void SetupScene(const char* filename);
	void RenderScene();
	// TODO: this should eventually be combined with RenderScene, but can only be called
	//   after all scene geometry is finished rendering
	void RenderSkybox(std::shared_ptr<ShaderProgram> shader);

private:
	/* ----- Objects that the GameEngine exclusively controls ----- */
	// TODO: do these need to be unique_ptrs, or can they just exist on the stack?
	std::unique_ptr<Window> mainWindow;
	std::unique_ptr<Scene> scene;
	std::unique_ptr<Skybox> skybox;

	/* ----- Objects that the GameEngine references, but have lifetimes controlled by
	other objects ----- */
	std::weak_ptr<Camera> cameraRef;
};

