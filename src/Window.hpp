#pragma once

#include <GLFW/glfw3.h>

/// 
/// Manages the current window & player inputs. Also sets up 
/// OpenGL function pointers with GLAD when initialized.
/// 
class Window {
public:
	Window(const int height, const int width, const char* title);
	~Window() = default;

	// Called when the user changes the window size
	void Resize(int width, int height);

	// TODO: this shouldn't be necessary
	GLFWwindow* GetWindow() const { return glfwWindow; }

private:
	/* Function callbacks - Reroutes to custom functions */
	static void ResizeCallback(GLFWwindow* window, int width, int height);

	GLFWwindow* glfwWindow = nullptr;
};

