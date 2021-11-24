#include "GameInstance.h"

#include <iostream>

GameInstance::GameInstance(const GameOptions options) {
	/* ----- Set up GLFW ----- */
	if (!glfwInit()) {
		std::cerr << "Error during GLFW Initialization!" << std::endl;
		abort();
	}
	// GLFW_CONTEXT_VERSION_MAJOR and GLFW_CONTEXT_VERSION_MINOR specify the 
	//    client API version that the created context must be compatible with.
	//    The exact behavior of these hints depend on the requested client API.
	// I.e. does the user have a recent-enough version of opengl installed?
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Use the (modern) core profile - don't include backwards-compatible features
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // only needed for Mac

	/* ----- Create the game window ----- */
	mainWindow = std::make_unique<Window>(options.windowWidth, options.windowHeight,
	                                      options.windowName);
	// Call the window's resizing function to initialize the OpenGL window size
	mainWindow->ResizeEvent(options.windowWidth, options.windowHeight);

	// Initialize GLAD so that OpenGL has the right function pointers.
	// Pass GLAD the function (from glfw) that loads the OpenGL function pointers
	// Note: there must be a current context on this thread for this to work
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "Error: Failed to initialize GLAD!" << std::endl;
		abort();
	}
}

GameInstance::~GameInstance() {
	// clean up all of GLFW's resources that were allocated
	glfwTerminate();
}
