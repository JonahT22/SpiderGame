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
	void ResizeEvent(int width, int height);
	// Called when any keyboard input is recieved. Use this for inputs from physical
	//   keys (on a US keyboard layout).
	void KeyEvent(int key, int scancode, int action, int mods);
	// Called when a Unicode character is pressed. Use this for text inputs,
	//   independent of keyboard layout.
	void CharEvent(unsigned int codepoint);
	// Called when the mouse is moved in the window
	void CursorPosEvent(double x_pos, double y_pos);
	// Called when a mouse button is clicked or released
	void MouseButtonEvent(int button, int action, int mods);

	// TODO: this shouldn't be necessary
	GLFWwindow* GetWindow() const { return glfwWindow; }

private:
	/* Function callbacks - Reroutes to custom functions */
	// Reroute the static callbacks to custom functions in a specific Window object 
	//   instance (this routing is set in the Window object's constructor)
	// Cast the user pointer (which is a reference to any class object) to a Window
	inline static void ResizeCallback(GLFWwindow* window, int width, int height) {
		Window* custom_window = static_cast<Window*>(glfwGetWindowUserPointer(window));
		custom_window->ResizeEvent(width, height);
	}
	inline static void KeyCallback(GLFWwindow* window, int key, int scancode,
	                               int action, int mods) {
		Window* custom_window = static_cast<Window*>(glfwGetWindowUserPointer(window));
		custom_window->KeyEvent(key, scancode, action, mods);
	}
	inline static void CharCallback(GLFWwindow* window, unsigned int codepoint) {
		Window* custom_window = static_cast<Window*>(glfwGetWindowUserPointer(window));
		custom_window->CharEvent(codepoint);
	}
	inline static void CursorPosCallback(GLFWwindow* window, double xpos,
	                                     double ypos) {
		Window* custom_window = static_cast<Window*>(glfwGetWindowUserPointer(window));
		custom_window->CursorPosEvent(xpos, ypos);
	}
	inline static void MouseButtonCallback(GLFWwindow* window, int button,
	                                       int action, int mods) {
		Window* custom_window = static_cast<Window*>(glfwGetWindowUserPointer(window));
		custom_window->MouseButtonEvent(button, action, mods);
	}

	GLFWwindow* glfwWindow = nullptr;
};
