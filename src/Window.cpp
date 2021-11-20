#include "Window.hpp"

#include <iostream>

Window::Window(const int width, const int height, const char* title) {
	// Make sure GLFW has been initialized
	glfwInit();

	// Create a window object
	glfwWindow = glfwCreateWindow(width, height, title, NULL, NULL);
	// Make the context of the window the main context on the current thread
	if (glfwWindow == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		abort();
	}
	glfwMakeContextCurrent(glfwWindow);

	std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;

	/* ----- Bind any input callbacks here ----- */
	// Associate this 'Window' instance with the glfwWindow that was created using
	//   the "user pointer", which is a way to reroute static glfw calls to a specific
	//   class instance.
	//   See https://stackoverflow.com/questions/55145966/what-does-glfwgetwindowuserpointer-do/55195385#55195385
	glfwSetWindowUserPointer(glfwWindow, this);
	glfwSetFramebufferSizeCallback(glfwWindow, ResizeCallback);
	glfwSetKeyCallback(glfwWindow, KeyCallback);
	glfwSetCharCallback(glfwWindow, CharCallback);
	glfwSetCursorPosCallback(glfwWindow, CursorPosCallback);
	glfwSetMouseButtonCallback(glfwWindow, MouseButtonCallback);
}

void Window::ResizeEvent(int width, int height) {
	// First params set the location of the lower left corner, next 2 set the width/height
	glViewport(0, 0, width, height);
}

void Window::KeyEvent(int key, int scancode, int action, int mods) {
	if ((key == GLFW_KEY_ESCAPE) && (action == GLFW_PRESS)) {
		glfwSetWindowShouldClose(glfwWindow, GL_TRUE);
	}
}

void Window::CharEvent(unsigned int codepoint) {
	// 'codepoint' is the Unicode codepoint of the character that was pressed
	// std::cout << "Char received: " << (char)codepoint << std::endl;
}

void Window::CursorPosEvent(double x_pos, double y_pos) {
	// std::cout << "Mouse is at (" << x_pos << ", " << y_pos << ")\n";
}

void Window::MouseButtonEvent(int button, int action, int mods) {
	// std::cout << "Mouse event recieved: " << button << std::endl;
}
