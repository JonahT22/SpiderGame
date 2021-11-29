#pragma once

#include <memory>

// Always include glad before glfw
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class Window;
class Camera;
class ShaderProgram;

// TODO: rendering options, like wireframe & backface culling
struct GameOptions {
	const int windowWidth = 256;
	const int windowHeight = 256;
	const char* windowName = "New Game Window";
};

/// 
/// Manages setup & lifetime for glfw, glad, and major game systems
/// 
/// TODO: probably should rename this class to "Renderer"
class GameInstance {
public:
	GameInstance(const GameOptions options);
	~GameInstance();
	
	// Getters
	// TODO: this shouldn't be necessary
	GLFWwindow* GetWindow() const;

	// Setters
	void SetCurrentCamera(const std::shared_ptr<Camera> new_camera);

	// Input events (from the mainWindow)
	void InputMoveCamera(glm::vec2 motion) const;

	// Other Functions
	void SetupScene(const char* filename);
	void RenderScene(ShaderProgram& shader);

private:
	// The gameinstance has exclusive control over the rendering window
	std::unique_ptr<Window> mainWindow;
	// Store a weak reference to the player's camera - it can disappear/change at any time
	std::weak_ptr<Camera> cameraRef;
};

