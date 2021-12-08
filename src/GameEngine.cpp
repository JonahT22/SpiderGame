#include "GameEngine.h"

#include <iostream>

#include <glad/glad.h>

#include "Camera.h"
// TODO: I shouldn't need to include these, but for some reason I do
#include "Skybox.h"
#include "ShaderProgram.h"

GameEngine::GameEngine(const GameOptions options) :
	physicsTimeStep(options.physicsTimeStep),
	frameDelayMs(options.frameDelayMs),
	showFramerate(options.showFramerate) {
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
	                                      options.windowName, this);

	// Initialize GLAD so that OpenGL has the right function pointers.
	// Pass GLAD the function (from glfw) that loads the OpenGL function pointers
	// Note: there must be a current context on this thread for this to work
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "Error: Failed to initialize GLAD!" << std::endl;
		abort();
	}

	// Once GLAD function pointers are loaded into the new context, set up the OpenGL viewport

	// Print OpenGL version. Don't need to print GLSL version since GLSL versions match
	//   with OpenGL for all versions after OpenGL 3.3
	std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;

	// Call the window's resizing function to initialize the OpenGL window size
	mainWindow->ResizeEvent(options.windowWidth, options.windowHeight);
	
	// Enable Z-buffer checking
	glEnable(GL_DEPTH_TEST);
	// Change the depth check from < to <=, so that skyboxes (which always have a depth
	//   of 1) can still pass depth tests
	glDepthFunc(GL_LEQUAL);

	// Set the void color
	glClearColor(options.clearColor.r, options.clearColor.g, options.clearColor.b, 1.0f);
}

GameEngine::~GameEngine() {
	// clean up all of GLFW's resources that were allocated
	glfwTerminate();
}

void GameEngine::SetupScene(const std::string& filename) {
	// Load the scene geometry from the file, which should also create the character
	//   and place it at the start location. Then, get a ref to the player's camera
	//   from the scene and hook up the window's input events to the character
	// TODO: might be useful to have a "playercontroller" class that has a
	//   "control rotation" and "control velocity" inputs that the camera & 
	//   character reference for their functions

	scene = std::make_unique<Scene>(enable_shared_from_this::weak_from_this());
	scene->LoadSceneFile(filename);
}

void GameEngine::RenderScene(double delta_time) {
	// Clear the color & depth buffers 
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	int run_this_frame = 0;
	while (physicsTimer > physicsTimeStep) {
		scene->UpdateScenePhysics();
		run_this_frame++;
		physicsTimer -= physicsTimeStep;
	}
	physicsTimer += delta_time;
	
	scene->RenderScene(frameDelayMs);

	// Swap OpenGL buffers
	glfwSwapBuffers(mainWindow->GetGLFWWindow()); // swap the color buffers
	// Check if any events been triggered
	glfwPollEvents();

	if (showFramerate) {
		std::cout << "Framerate: " << 1.0f / delta_time << std::endl;
	}
}

void GameEngine::InputMoveCamera(glm::vec2 motion) const {
	if (!cameraRef.expired()) {
		cameraRef.lock()->ApplyRotationInput(motion);
	}
}

void GameEngine::UpdateCameraAspect(const float new_aspect) const {
	if (!cameraRef.expired()) {
		cameraRef.lock()->SetAspectRatio(new_aspect);
	}
}

const std::unique_ptr<Window>& GameEngine::GetWindow() const {
	return mainWindow;
}

std::shared_ptr<Camera> GameEngine::GetMainCamera() {
	// Note: This function cannot be declared as const. A const specifier prevents passing
	//   a non-const weak_from_this to the empty camera constructor
	if (std::shared_ptr<Camera> camera = cameraRef.lock())
		return camera;
	else {
		std::cerr << "ERROR: Tried to get current camera, but there is no camera set in";
		std::cerr << " the game engine!" << std::endl;
		// Construct an empty camera object
		return std::make_shared<Camera>(enable_shared_from_this::weak_from_this(),
		                                "null_camera");
	}
}

bool GameEngine::IsWindowOpen() const {
	return !glfwWindowShouldClose(mainWindow->GetGLFWWindow());
}

bool GameEngine::IsKeyPressed(const int key) const {
	// Ignore keys outside the range of the key array
	if (key < 0 || key >= GLFW_KEY_LAST) {
		std::cerr << "ERROR: IsKeyPressed - Invalid key!" << std::endl;
		return false;
	}
	return keysPressed[key];
}

void GameEngine::SetCurrentCamera(const std::shared_ptr<Camera> new_camera) {
	cameraRef = new_camera;
}

void GameEngine::SetKeyPressed(int key, bool is_pressed) {
	// Ignore keys outside the range of the key array
	if (key < 0 || key >= GLFW_KEY_LAST) {
		std::cerr << "ERROR: SetKeyPressed - Invalid key!" << std::endl;
		return;
	}
	keysPressed[key] = is_pressed;
}
