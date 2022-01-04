#pragma once

#include <memory>
#include <string>

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "Utils/GameOptions.h"
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
	std::string GetDefaultModelPath() const;

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

	// Counter for keeping track of physics updates
	float physicsTimer = 0.0f;
	
	GameOptions options;

	// Callback for any opengl debug_output messages
	static void APIENTRY DebugCallback(GLenum source, GLenum type, GLuint id,
		GLenum severity, GLsizei length, const GLchar* message, const void* userParam );
};

