#include <iostream>

#include <glad/glad.h>

#include "Player/Camera.h"
#include "GameEngine.h"
#include "Utils/GameOptions.h"
#include "Rendering/Scene.h"
#include "Rendering/Window.h"
// TODO: I shouldn't need to include these, but for some reason I do
#include "Rendering/Skybox.h"
#include "Rendering/ShaderProgram.h"

GameEngine::GameEngine(const std::string& options_file) :
	options(options_file) {
	/* ----- Set up GLFW ----- */
	if (!glfwInit()) {
		std::cerr << "Error during GLFW Initialization!" << std::endl;
		abort();
	}
	// GLFW_CONTEXT_VERSION_MAJOR and GLFW_CONTEXT_VERSION_MINOR specify the 
	//    client API version that the created context must be compatible with.
	//    The exact behavior of these hints depend on the requested client API.
	// I.e. does the user have a recent-enough version of opengl installed?
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Use the (modern) core profile - don't include backwards-compatible features
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // only needed for Mac
	// Add debug support (Note: SIGNIFICANTLY slows down program, so TODO make this
	//   optional later
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

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

	// Print OpenGL version. Don't need to print GLSL version since GLSL versions match
	//   with OpenGL for all versions after OpenGL 3.3
	std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;

	// Set up debug_output extension
	int flags;
	glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
	if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
		// Set opengl debug messages to use a custom callback function
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); 
		glDebugMessageCallback(DebugCallback, nullptr);
		// Optionally set filters for messages. GL_DONT_CARE allows any messages
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
	}

	// Call the window's resizing function to initialize the OpenGL window size
	mainWindow->ResizeEvent(options.windowWidth, options.windowHeight);
	
	// Enable Z-buffer checking
	glEnable(GL_DEPTH_TEST);
	// Change the depth check from < to <=, so that skyboxes (which always have a depth
	//   of 1) can still pass depth tests
	glDepthFunc(GL_LEQUAL);

	// Set the debug message callback
	glEnable(GL_DEBUG_OUTPUT);

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

	scene = std::make_shared<Scene>(enable_shared_from_this::weak_from_this());
	scene->LoadSceneFile(filename);
}

void GameEngine::RenderScene(double delta_time) {
	// Clear the color & depth buffers 
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	int run_this_frame = 0;
	while (physicsTimer > options.physicsTimeStep) {
		scene->UpdateScenePhysics(options.physicsTimeStep);
		run_this_frame++;
		physicsTimer -= options.physicsTimeStep;
	}
	physicsTimer += delta_time;
	
	scene->RenderScene(options.frameDelayMs);

	// Swap OpenGL buffers
	glfwSwapBuffers(mainWindow->GetGLFWWindow()); // swap the color buffers
	// Check if any events been triggered
	glfwPollEvents();

	if (options.showFramerate) {
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
	if (std::shared_ptr<Camera> camera = cameraRef.lock()) {
		return camera;
	}
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

const float GameEngine::GetPhysicsTimeStep() const {
	return options.physicsTimeStep;
}

std::string GameEngine::GetDefaultModelPath() const {
	return options.defaultModelPath;
}

std::shared_ptr<Scene> GameEngine::GetCurrentScene() const {
	return scene;
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

void APIENTRY GameEngine::DebugCallback(GLenum source, GLenum type, GLuint id,
	GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
	// Debug callback adapted from https://learnopengl.com/In-Practice/Debugging
	
	// Ignore non-significant error/warning codes
	if(id == 131169 || id == 131185 || id == 131218 || id == 131204) {
		return;
	}

	std::cout << "---------------" << std::endl;
	std::cout << "Debug message (" << id << "): " <<  message << std::endl;

	switch (source) {
		case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
		case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
		case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
		case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
	}
	std::cout << std::endl;

	switch (type) {
		case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break; 
		case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
		case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
		case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
		case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
		case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
		case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
	}
	std::cout << std::endl;
	
	switch (severity) {
		case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
		case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
		case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
		case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
	}
	std::cout << std::endl << std::endl;
}