#include "GameEngine.h"

#include <iostream>

// Always include glad before glfw
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
//TODO remove
#include <glm/gtx/string_cast.hpp>

#include "Camera.h"
#include "ShaderProgram.h"
#include "Skybox.h"
#include "Window.h"

GameEngine::GameEngine(const GameOptions options) {
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
	// Call the window's resizing function to initialize the OpenGL window size
	mainWindow->ResizeEvent(options.windowWidth, options.windowHeight);

	// Initialize GLAD so that OpenGL has the right function pointers.
	// Pass GLAD the function (from glfw) that loads the OpenGL function pointers
	// Note: there must be a current context on this thread for this to work
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "Error: Failed to initialize GLAD!" << std::endl;
		abort();
	}
	
	// Enable Z-buffer checking
	glEnable(GL_DEPTH_TEST);
	// Change the depth check from < to <=, so that skyboxes (which always have a depth
	//   of 1) can still pass depth tests
	glDepthFunc(GL_LEQUAL);
}

GameEngine::~GameEngine() {
	// clean up all of GLFW's resources that were allocated
	glfwTerminate();
}

GLFWwindow* GameEngine::GetWindow() const {
	return mainWindow->GetWindow();
}

std::shared_ptr<Camera> GameEngine::GetMainCamera() const {
	if (std::shared_ptr<Camera> camera = cameraRef.lock())
		return camera;
	else {
		std::cerr << "ERROR: Tried to get current camera, but there is no camera set in";
		std::cerr << " the game engine!" << std::endl;
		// 
		return std::make_shared<Camera>(enable_shared_from_this::weak_from_this());
	}
}

void GameEngine::SetCurrentCamera(const std::shared_ptr<Camera> new_camera) {
	cameraRef = new_camera;
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

void GameEngine::SetupScene(const char* filename) {
	// Load the scene geometry from the file, which should also create the character
	//   and place it at the start location. Then, get a ref to the player's camera
	//   from the scene and hook up the window's input events to the character
	// TODO: might be useful to have a "playercontroller" class that has a
	//   "control rotation" and "control velocity" inputs that the camera & 
	//   character reference for their functions

	// TODO: load the cubemap image paths from the scene file
	// TODO: detect image file type, choose RGB or RGBA in texture loader
	const char* cube_map_image_paths[6] = {
		"resources/skybox1/right.jpg",
		"resources/skybox1/left.jpg",
		"resources/skybox1/top.jpg",
		"resources/skybox1/bottom.jpg",
		"resources/skybox1/front.jpg",
		"resources/skybox1/back.jpg",
	};
	skybox = std::make_unique<Skybox>(cube_map_image_paths);
}

void GameEngine::RenderScene(std::shared_ptr<ShaderProgram> shader) {
	// TODO: later, this will do all of the rendering commands. For now, just use
	//   it to test getting the view/projection matrices from the camera

	// Try to get a reference to the camera. Only proceed if it is valid
	if (std::shared_ptr<Camera> currentCamera = cameraRef.lock()) {
		// Send camera matrices to the shader
		shader->Activate();
		shader->SetMat4UniformPtr("V", currentCamera->GetViewMtxPtr());
		shader->SetMat4UniformPtr("V_invT", currentCamera->GetViewMtxInvTPtr());
		shader->SetMat4UniformPtr("P", currentCamera->GetProjectionMtxPtr());
	}
	else {
		std::cerr << "ERROR: No camera set in the game instance!" << std::endl;
	}
}

void GameEngine::RenderSkybox(std::shared_ptr<ShaderProgram> shader) {
	// TODO: this should re-use the camera ref from before
	auto main_camera = GetMainCamera();
	if (main_camera) {
		// Send camera matrices to the shader
		shader->Activate();
		// Remove the translation factors from the view matrix by casting to a mat3
		// This works because the mat3->mat4 conversion places a 1 into unfilled
		//   diagonals, essentially setting the last column to (0, 0, 0, 1)
		shader->SetMat4Uniform("V", glm::mat4(glm::mat3(currentCamera->GetViewMtx())));
		shader->SetMat4UniformPtr("P", currentCamera->GetProjectionMtxPtr());
		glm::mat4 view = glm::mat4(glm::mat3(main_camera->GetViewMtx()));
	}
	else {
		std::cerr << "ERROR: No camera set in the game instance!" << std::endl;
	}
	// Note: the skybox MUST be drawn last
	skybox->Render();
}
