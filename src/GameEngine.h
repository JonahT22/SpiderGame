#pragma once

#include <memory>
#include <string>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class Window;
class Scene;
class Camera;

///
/// Handles rendering frames, calling physics updates, managing the
///   main rendering window and currently-active scene, and sending player
///   inputs to the correct objects
///
/// Note: GameEngine must inherit from enable_shared_from_this so that it can populate
///   newly-created SceneObjects with weak references to this GameEngine
class GameEngine : public std::enable_shared_from_this<GameEngine> {
public:
	GameEngine(const std::string& options_file);
	~GameEngine();

	void SetupScene(const std::string& filename);
	void RenderScene(double delta_time);

	/* ----- Input events (from the mainWindow) ----- */
	void InputMoveCamera(glm::vec2 motion) const;
	void UpdateCameraAspect(const float new_aspect) const;

	/* ----- Getters ----- */
	const std::unique_ptr<Window>& GetWindow() const;
	std::shared_ptr<Camera> GetMainCamera();
	bool IsWindowOpen() const;
	bool IsKeyPressed(const int key) const;
	const float GetPhysicsTimeStep() const;

	/* ----- Setters ----- */
	void SetCurrentCamera(const std::shared_ptr<Camera> new_camera);
	void SetKeyPressed(int key, bool is_pressed);

private:
	/* ----- Objects that the GameEngine exclusively controls ----- */
	// TODO: do these need to be unique_ptrs, or can they just exist on the stack?
	std::unique_ptr<Window> mainWindow;
	std::unique_ptr<Scene> scene;

	/* ----- Objects that the GameEngine references, but have lifetimes controlled by
	other objects ----- */
	std::weak_ptr<Camera> cameraRef;

	/* ----- Keyboard Inputs ----- */
	// Keep track of which keys are currently being pressed
	bool keysPressed[GLFW_KEY_LAST] = {false};

	/* ----- Framerate and display settings ----- */
	// Desired period for the physics updates, and a running timer for the next physics step
	float physicsTimeStep = 0.1f;
	float physicsTimer = 0.0f;
	// Artificial delay on each rendering frame, for testing physics behavior
	unsigned int  frameDelayMs = 0;
	// Should the framerate be printed to the log?
	bool showFramerate = false;
};

