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
	inline static void ResizeCallback(GLFWwindow* window, int width, int height) {
		// Reroute the static resize callback to a custom Resize Function
		//   in a specific Window object instance (this routing is set
		//   in the Window object's constructor)
		// Cast the user pointer (which is a reference to any class object) to a Window
		Window* customWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
		customWindow->Resize(width, height);
	}

	GLFWwindow* glfwWindow = nullptr;
};

