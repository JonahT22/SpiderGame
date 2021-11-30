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
/// Manages setup & lifetime for glfw, glad, and major game systems
///
class GameInstance {
public:
	GameInstance(const GameOptions options);
	~GameInstance();
	
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

