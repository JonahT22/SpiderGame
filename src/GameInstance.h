#pragma once

#include <memory>

// Always include glad before glfw
#include <glad/glad.h> 
#include <GLFW/glfw3.h>

#include "Window.hpp"

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
	// TODO: this shouldn't be necessary
	GLFWwindow* GetWindow() const { return mainWindow->GetWindow(); }

private:
	std::unique_ptr<Window> mainWindow;
};

