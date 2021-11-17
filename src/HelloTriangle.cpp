#include <glad/glad.h> // glad manages interfacing with drivers to get specific opengl implementations of functions
// include glad before glfw
// GLFW handles the opengl context, window management, and user input
#include <GLFW/glfw3.h>
#include <iostream>

// create a callback function that runs whenever the window gets resized
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height); // re-specify the viewport's size
}

// handle user input using glfw's getkey function
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true); // close the window when esc key pressed
}

// For now, just store the shader code in a const string at the top of the file
const char* vertexShaderSource = 
	"#version 330 core\n" // opengl version 3.3, using the core profile
	// create a vertex attribute aPos
	// the position variable has attribute position 0
	"layout (location = 0) in vec3 aPos;\n"
	// the color variable has attribute position 1
	"layout (location = 1) in vec3 aColor;\n"
	// specify a color output to send to the fragment shader
	// NOTE: some ppl say to use the 'varying' keyword instead of in/out. This is now deprecated
	// see: https://stackoverflow.com/questions/34627576/why-did-glsl-change-varying-to-in-out
	"out vec3 ourColor;"
	"void main()\n"
	"{\n"
	// gl_Position is a built-in variable, and the final output of a vertex shader
	// Note that our vertex data is already in NDC, but usually we'd have to transform
	// from world -> normalized device coords so that they fall within OpenGL's visible region
	"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n" // vec4, with w = 1.0
	"	ourColor = aColor;\n" // set ourColor to the input color we got from the vertex data
	"}\0";
const char* fragmentShaderSource =
	"#version 330 core\n" // see above^
	"out vec4 FragColor;\n" // frag shader must ALWAYS output a vec4 for color
	"in vec3 ourColor;" // use same and type as vert shader, OpenGL will link them together
	//"uniform vec4 ourColor;\n"
	"void main()\n"
	"{\n"
	//"	FragColor = vertexColor;\n" 
	"	FragColor = vec4(ourColor, 1.0);\n"
	"}\0";

int main()
{
	// ----------------------- HELLO WINDOW ----------------------------------
	glfwInit();  // initialize GLFW
	// from https:// www.glfw.org/docs/latest/window.html#window_hints
	// GLFW_CONTEXT_VERSION_MAJORand GLFW_CONTEXT_VERSION_MINOR specify the client API version that the created context must be compatible with.
	// The exact behavior of these hints depend on the requested client API.
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // make sure the user is using OpenGL 3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// specifies which OpenGL profile to create the context for. 
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // explicitly use the core profile - don't include backwards-compatible features
	// glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // only needed for Mac

	// Next, create a window object
	GLFWwindow* window = glfwCreateWindow(800, 600, "Getting Started", NULL, NULL);
	// make the context of the window the main context on the current thread
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLAD so that OpenGL has the right function pointers
	// pass GLAD the function (from glfw) to load the address of the OpenGL function pointers
	// a.k.a let GLAD handle loading the openGL function pointers instead of glfw?
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// Tell openGL the size of the rendering window (viewport)
	// 1st 2 params set the location of the lower left corner, next 2 set the width/height
	glViewport(0, 0, 800, 600); // I think this is optional, since the framebuffer_size_callback should handle this
	// bind the resizing function to the 'window resizing' action
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	// in general, this is where we'll want to register any other callbacks, like joystick input


	// ----------------------- HELLO TRIANGLE ------------------------
	// Create the Vertex Shader
	unsigned int vertexShader; // the shader is an OpenGL object, so it has an ID
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	// Attach the shader source code to the shader object
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	// compile the shader
	glCompileShader(vertexShader);
	// Check if the shader compilation was successful
	int success; // create variables to pass by reference
	char infoLog[512]; // storage container for error msg
	// check for successful compilation
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	// if failed, retrieve error msg
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// Create the fragment shader (color calculation)
	// Use the same process as the vertex shader
	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	// check for successful compilation
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	// if failed, retrieve error msg
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// Link the shaders together into a shader program (multiple shaders linked together)
	// Note: when linking, it links the OUTPUTS of each shader to the INPUTS of the NEXT shader
	unsigned int shaderProgram; // object's ID
	shaderProgram = glCreateProgram(); // create the program
	// Attach the shaders to the program
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	// Link the two attached shaders together
	glLinkProgram(shaderProgram);
	// handle error checking, just like before
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// finally, activate the program
	glUseProgram(shaderProgram);
	// Now, every shader and rendering call will use this program object (and our custom shaders)
	// Since the shaders are now linked to the program, we don't need them anymore
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	
	// So, create an array of vertices in Normalized Device Coordinates (range -1 to 1)
	// the glviewport command is how opengl converts between NDC and the screen space coords (which have their origin in top left)
	// the last 3 values in each row are color data
	float vertices[] = {
	 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f, //  top right
	 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f, //  bottom right
	-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f, //  bottom left
	-0.5f,  0.5f, 0.0f,   0.0f, 0.0f, 0.0f  //  top left 
	};
	// tells openGL which vertices to draw in what order (used by the Element Buffer Object)
	unsigned int indices[] = {  //  note that we start from 0!
		0, 1, 3,   //  first triangle
		1, 2, 3    //  second triangle
	};
	// Create an Element Buffer Object to hold some hardcoded points
	// EBOs are ways to specify both the vertices, and the order to draw them. This lets you do things like draw a rectangle
	// by only specifying 4 points (otherwise, you'd have to define two triangles: 6 points, 2 of them identical)
	unsigned int EBO;
	glGenBuffers(1, &EBO); // first arg is the # of buffers to gen. check out docs, but not often used afaik
	
	// Create a Vertex Buffer Object (VBO):
	// A VBO is used to send a big set of data to the GPU all at once
	// ALL openGL objects must have a unique ID
	unsigned int VBO; // id of the VBO
	glGenBuffers(1, &VBO); // generate a buffer, storing its ID in VBO
	/*
	// next, bind this buffer as the GL_ARRAY_BUFFER. OpenGL can only have 1 GL_ARRAY_BUFFER (a.k.a vertex buffer),
	// so this command selects VBO as the GL_ARRAY_BUFFER target
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// Now, anything we do with GL_ARRAY_BUFFER will configure VBO
	// Copy the vertex data we hardcoded into the buffer VBO
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// ^ 4th parameter: how should the graphics card manage the data?
	//  GL_STREAM_DRAW: the data is set only once and used by the GPU at most a few times.
	//  GL_STATIC_DRAW : the data is set only once and used many times.
	//  GL_DYNAMIC_DRAW : the data is changed a lot and used many times

	// Link the Vertex Attributes
	// We need to specify how openGL should read our vertex buffer. We have it set up as a linear
	// array of floats, with each vertex having 3 entries. An 'attribute' is how we specify our inputs
	// I.e. 1 attribute is an x, y, z location in our float array. Vertex attributes could also be the 
	// colors / normals at each vertex
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0); // vertex attributes are disabled by default
	// Explanation of parameters: 
		// GLuint index: We set the "position" attribute (aPos) in the vertex shader to 0. We set this
		//               index to 0 to match that, so we're setting attribute 0 (which is aPos)
		// GLint size: This attr is a vec3, so it has 3 values
		// GLenum type: we're reading an array of floats
		// GLboolean normalized: do we want to normalize this? We don't need to since our input
		//                       float is already normalized
		// GLsizei stride: The space btwn consecutive vertex attributes (in bytes). We could also
		//                 set this to 0 if we know the array is tightly packed (ours is)
		// const void* pointer: the offset of where the position data begins in the buffer.
		//                      Our data is at the start of the array, so this is 0
	// Remember, we set VBO as the current GL_ARRAY_BUFFER, which is where we're pulling the 
	// data for the attributes when we call glVertexAttribPointer

	// ^all of this is a long way to do it. We have to call:
	// glBindBuffer
	// glBufferData
	// glVertexAttribPointer
	// glEnableVertexAttribArray
	// glUseProgram
	// Then, we can actually draw the object. We'd need to repeat this for EVERY new object. Use a VAO instead
	*/

	// Set up a Vertex Array Object (VAO)
	// Basically, all the states we just set can be stored in the VAO, so we only have to bind the VAO when
	// switching to a different set of vertex data/attribute configurations
	// NOTE: OpenGL 'core' requires you to use VAOs
	// Each object that you draw will have its own VAO
	// Usually, you want to generate the VAOs for all the objects you want at the start of the program, and store them for later use
	unsigned int VAO;
	glGenVertexArrays(1, &VAO); // generate the Vertex Array Object
	glBindVertexArray(VAO); // bind our new vertex array object
	// Repeat the code from earlier where we set up the buffer and attributes
	// copy the vertices array in a buffer for OpenGL to use
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// Make the EBO. Just like the VBO, bind the element buffer, add stuff to it, then unbind it
	// note: the last element array buffer object bound to a VAO gets stored as that VAO's element buffer object.
	// from now on, binding to the VAO will also bind to this EBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	// Set our vertex attributes pointers
	// Position Attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// Color Attribute
	//NOTE: since the color starts after the 3rd element, our *pointer is also offset by 3 floats
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// Now, if we want to draw an object, we just bind VAO and draw it using some opengl draw function
	// See the render loop for how to actually use this
	// IMPORTANT NOTE: a VAO can only have 1 EBO and 1 VBO. It can have many different (16?) attributes pointers (each one of which points 
	// to a different spot in the VBO). The EBO doesn't have attributes like the VBO does (I think)

	// Optional: you can unbind the VBO and VAO so that further calls don't modify them. In general, THIS IS NOT NECESSARY
	// It's safe to unbind VBO here; the call to glVertexAttribPointer registered VBO as VAO's bound vertex buffer object
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// next, we can unbind VAO so that we don't accidentally modify if later
	glBindVertexArray(0);
	// ONLY AFTER VAO is unbound, we can unbind the element buffer. You CANNOT unbind this while VAO is active, or VAO will stop 
	// recognizing it as the current EBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	// ^although, I tried moving this a few lines up and it still worked so idk

	//  uncomment this call to draw in wireframe polygons.
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
	// ------------------------------------- RENDER LOOP ------------------------------------------------------
	while (!glfwWindowShouldClose(window)) // has GLFW been told to close?
	{
		// input
		processInput(window); // handle user input using a custom function (this isn't built-in)

		// rendering commands
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // set the void color to dark green-blue (STATE-SETTING function)
		glClear(GL_COLOR_BUFFER_BIT); // only clear the color buffer, not depth or stencil buffer (STATE-USING function)
		
		// ---------------------------SHADERS--------------------------
		/* Practice with uniforms: 
		float timeValue = glfwGetTime();
		// interp a value from 0 to 1
		float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
		// Get the location of the uniform variable that we defined in the frag shader (-1 means not found)
		int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
		// set the shader (MUST be done before updating the uniform's value)
		glUseProgram(shaderProgram); 
		// set 'ourColor' to just green (alpha = 1). Note: uniform4f sets a value on the CURRENT shader, so have to set the shader first
		glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f); 
		// ^ in general, anytime a function is overloaded it will have a postfix for the type (OpenGL is built in C, so no overload support)
		// ex: glUniformfv = float vector/array (we could also use that here), glUniformui = unsigned int, etc...
		*/
		glUseProgram(shaderProgram);
		// Draw our triangle using our Vertex Array Object (VAO)
		// bind our VAO, which has all of the buffer/attribute settings. Since we only have 1 object it isn't
		// strictly necessary that we bind it each time, but it's a good general practice
		glBindVertexArray(VAO); 
		/* This code is for if we want to draw straight from the vertex buffer
		glDrawArrays(GL_TRIANGLES, 0, 3);
		// ^ 1: the primitive type that we want to draw
		//   2: The starting index of the vertex array that we want to draw
		//   3: How many vertices we want to draw
		*/
		// Instead, let's draw from the element buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		// ^ 1: the primitive type (just like VBO)
		//   2: # of elements to draw (we have 6 indices, so draw 6 vertices
		//   3: the type of the indices
		//   4: offset or array ref, we don't need to worry abt it now

		// check and call events, swap buffers
		glfwSwapBuffers(window); // swap the color buffers
		glfwPollEvents(); // have any events been triggered (i.e. input?)
	}

	//  optional: de-allocate all resources once they've outlived their purpose:
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(shaderProgram);

	glfwTerminate(); // clean up all of GLFW's resources that we allocated
	return 0;
}