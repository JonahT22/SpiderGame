#pragma once

#include <memory>

#include <glm/glm.hpp>

class Window;
class Camera;
class ShaderProgram;
struct GLFWwindow;

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
class GameEngine {
public:
	GameEngine(const GameOptions options);
	~GameEngine();
	
	// Getters
	GLFWwindow* GetWindow() const;

	// Setters
	void SetCurrentCamera(const std::shared_ptr<Camera> new_camera);

	// Input events (from the mainWindow)
	void InputMoveCamera(glm::vec2 motion) const;
	void UpdateCameraAspect(const float new_aspect) const;

	// Other Functions
	void SetupScene(const char* filename);
	void RenderScene(std::shared_ptr<ShaderProgram> shader);

private:
	// The gameinstance has exclusive control over the rendering window
	std::unique_ptr<Window> mainWindow;
	// Store a weak reference to the player's camera - it can disappear/change at any time
	std::weak_ptr<Camera> cameraRef;
};

