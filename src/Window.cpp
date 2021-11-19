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

	/* ----- Bind any input callbacks here ----- */
	glfwSetFramebufferSizeCallback(glfwWindow, ResizeCallback);
}

void Window::Resize(int width, int height) {
	// First params set the location of the lower left corner, next 2 set the width/height
	glViewport(0, 0, width, height);
}

void Window::ResizeCallback(GLFWwindow* window, int width, int height) {
	// TODO: this should just reroute the call to the resize() function
	// First params set the location of the lower left corner, next 2 set the width/height
	glViewport(0, 0, width, height);
}
