#include "Window.h"

#include <glad/glad.h>

#include "GameEngine.h"

Window::Window(const int width, const int height, const char* title,
               GameEngine* engine) : engineRef(engine) {
	// Note: GLFW should be initialized at this point, and do NOT call any OpenGL functions
	//   in this constructor since the GameEngine hasn't yet initialized GLAD

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
	glfwSetWindowFocusCallback(glfwWindow, FocusCallback);
	glfwSetErrorCallback(ErrorCallback);
}

void Window::ResizeEvent(int width, int height) {
	// First params set the location of the lower left corner, next 2 set the width/height
	glViewport(0, 0, width, height);
	if (engineRef != nullptr) {
		engineRef->UpdateCameraAspect(width / (float)height);
	}
	else {
		std::cerr << "ERROR: Invalid GameEngine ref on window object!" << std::endl;
	}
}

void Window::KeyEvent(int key, int scancode, int action, int mods) {
	if ((key == GLFW_KEY_ESCAPE) && (action == GLFW_PRESS)) {
		glfwSetWindowShouldClose(glfwWindow, GL_TRUE);
	}
	// Don't send any GLFW_REPEAT events to the gameengine, just press and release
	else if ((action == GLFW_PRESS) || (action == GLFW_RELEASE)) {
		if (engineRef != nullptr) {
			engineRef->SetKeyPressed(key, (action == GLFW_PRESS));
		}
		else {
			std::cerr << "ERROR: Invalid GameEngine ref on window object!" << std::endl;
		}
	}
}

void Window::CharEvent(unsigned int codepoint) {
	// 'codepoint' is the Unicode codepoint of the character that was pressed
	// std::cout << "Char received: " << (char)codepoint << std::endl;
}

void Window::CursorPosEvent(double x_pos, double y_pos) {
	// Note: This cursor position event will still be called even if the window isn't
	//   in focus, so check whether the cursor is captured first
	if (cursorCaptured) {
		if (engineRef != nullptr) {
			glm::vec2 new_pos(x_pos, y_pos);
			engineRef->InputMoveCamera(new_pos - mousePos);
			mousePos = new_pos;
		}
		else {
			std::cerr << "ERROR: Invalid GameEngine ref on window object!" << std::endl;
		}
	}
}

void Window::MouseButtonEvent(int button, int action, int mods) {
	// std::cout << "Mouse event recieved: " << button << std::endl;
	
	// Check if the cursor has already been captured
	cursorCaptured = (glfwGetInputMode(glfwWindow, GLFW_CURSOR) == GLFW_CURSOR_DISABLED);
	if (!cursorCaptured) {
		// If capturing the mouse cursor for the first time, initialize mouse position
		double x, y;
		glfwGetCursorPos(glfwWindow, &x, &y);
		mousePos = glm::vec2(x, y);
		// Hide the mouse cursor and set the 'captured' flag
		glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		cursorCaptured = true;
	}
}

void Window::FocusEvent(int focused) {
	// 'focused' is either GLFW_TRUE or GLFW_FALSE
	if (focused == GLFW_FALSE) {
		// Release the mouse cursor when focus is lost
		glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		cursorCaptured = false;
	}
}

GLFWwindow* Window::GetGLFWWindow() const {
	return glfwWindow;
}

float Window::GetAspect() const {
	int width, height;
	glfwGetWindowSize(glfwWindow, &width, &height);
	float aspect = width / (float)height;
	return aspect;
}
