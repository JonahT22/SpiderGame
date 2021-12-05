#include "Mesh.h"

#include <iostream>

#include "ShaderProgram.h"

Mesh::Mesh(std::weak_ptr<GameEngine> engine, const std::string& name) :
	SceneObject(engine, name),
	vertexArrayID(0),
	vertexBufferID(0),
	elementBufferID(0),
	useEBO(false)
{

}

Mesh::~Mesh() {
	// Deallocate all of the buffers and arrays
	glDeleteVertexArrays(1, &vertexArrayID);
	glDeleteBuffers(1, &vertexBufferID);
	glDeleteBuffers(1, &elementBufferID);
}

void Mesh::LoadMesh(const std::string& filename, const bool use_EBO) {
	std::cerr << "ERROR: Mesh::LoadMesh() is not yet implemented!" << std::endl;
	abort();

	/* ----- Load vertex data from file ----- */

	/* ----- If the file has index data, load the EBO ----- */
	useEBO = use_EBO;
	if (useEBO) {
		// The EBO tells openGL which vertices to draw in what order
		// Hardcoded points to show the general idea:
		GLuint indices[] = {
			// These are hardcoded indices for a plane
			// Note that we start from 0!
			0, 1, 3,  // first triangle
			1, 2, 3   // second triangle
		};
		elementBuffer.assign(std::begin(indices), std::end(indices));
	}
	SetupVertexArray();
}

void Mesh::GenerateCubeMesh() {
	useEBO = false;
	// Specify the positions in Normalized Device Coordinates (range -1 to 1,
	//   where 0,0,0 is the center of the world)
	// OpenGL converts this to screen-space coordinates (where 0,0 = top left)
	//   using the info provided in glviewport commands
	GLfloat positions[] = {
		// Vertex data for a cube
		// posX, posY, posZ,  norX,  norY,  norZ, texU, texV,
		// Back
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f,
		// Front				     
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f,
		// Left
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
		// Right				    
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
		// Bottom				    
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f,
		// Top					    
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f
	};
	vertexBuffer.assign(std::begin(positions), std::end(positions));

	SetupVertexArray();
}

void Mesh::LoadTexture(const std::string& filename) {
	texture.LoadFromFile(filename);
}

void Mesh::Render(const std::shared_ptr<ShaderProgram> shader) const {
	// Call the parent's render function, which checks if the shader is bound and sends
	//   the model/invT matrices
	SceneObject::Render(shader);

	// Set the mesh's texture to texture unit 0 (default)
	if (texture.IsLoaded()) {
		texture.Bind();
	}
	// Load this mesh's buffer/attribute settings
	glBindVertexArray(vertexArrayID);
	// Draw the mesh using either indexed EBO drawing or raw VBO drawing
	if (useEBO) {
		// Draw the mesh using indexed drawing & the element buffer
		glDrawElements(GL_TRIANGLES, elementBuffer.size(), GL_UNSIGNED_INT, 0);
		// ^ 1: the primitive type (just like the VBO DrawArrays version)
		//   2: # of elements to draw (we have 6 indices, so draw 6 vertices
		//   3: the type of the indices
		//   4: offset or array ref, we don't need to worry abt it now
	}
	else {
		// Draw straight from the VBO (w/out indexed drawing)
		glDrawArrays(GL_TRIANGLES, 0, vertexBuffer.size() / 8);
		// ^ 1: the primitive type that we want to draw
		//   2: The starting index of the vertex array that we want to draw
		//   3: How many vertices we want to draw
	}
}

void Mesh::SetupVertexArray() {
	/* ----- Create the vertex array object ----- */
	// VAO - Stores the buffer & attribute configurations for this object, so you
	//   don't have to re-specify how the attributes are set up each time you draw
	//   the object (just bind the VAO and you're good)
	// Usually, you want to generate the VAOs for all the objects you want at the
	//   start of the program, and store them for later use
	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);

	/* ----- Create the buffer objects ----- */
	// Main VBO
	glGenBuffers(1, &vertexBufferID);
	// Optional EBO
	if (useEBO) {
		// Create the EBO - a way to specify both the vertices, and the order to draw them.
		//   This lets you do things like draw a rectangle by only specifying 4 points
		//   (otherwise, you'd have to define two triangles: 6 points,
		//   2 of them identical)
		glGenBuffers(1, &elementBufferID);

	}
	
	/* ----- Send the buffers to the GPU and set their attribute pointers ----- */
	// GENERAL EXPLANATION & NOTES:
	// glBindBuffer - Selects a buffer as the current VBO for OpenGL to modify
	// glBufferData - Send the data to the GPU. Fourth parameter is important:
	//   GL_STREAM_DRAW: the data is set only once and used by the GPU at most a few times.
	//   GL_STATIC_DRAW : the data is set only once and used many times.
	//   GL_DYNAMIC_DRAW : the data is changed a lot and used many times
	// glVertexAttribPointer - Link the Vertex Attributes to a certain "location"
	//   We need to specify how openGL should read our vertex buffer. We have it
	//   set up as a linear array of floats, with each vertex having 3 entries.
	//    An 'attribute' is how we specify our inputs
	//      I.e. 1 attribute is an x, y, z location in our float array. Vertex
	//      attributes could also be the colors / normals at each vertex
	//   Explanation of parameters: 
	//     GLuint index: We set the 'position' attribute (aPos) in the vertex shader
	//                   to location = 0. We set this index to 0 to match that,
	//                   so we're setting attribute 0 (which is aPos)
	//     GLint size: This attr is a vec3, so it has 3 values
	//     GLenum type: We're reading an array of floats
	//     GLboolean normalized: Do we want to normalize this? We don't need to
	//                           since our input float is already normalized
	//     GLsizei stride: The space btwn consecutive vertex attributes (in bytes).
	//                     We could also set this to 0 if we know the array is
	//                     tightly packed (ours is)
	//     const void* pointer: The offset of where the position data begins in the
	//                          buffer. Our data is at the start of the array,
	//                          so this is 0
	//   Remember, we set VBO as the current GL_ARRAY_BUFFER, OpenGL looks at the
	//     currently-bound array buffer to get the data for the attributes when
	//     we call glVertexAttribPointer
	// glEnableVertexAttribArray - Tell OpenGL there is an attribute at this location

	// Send vertex data to the GPU
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, vertexBuffer.size() * sizeof(GLfloat), 
	             &vertexBuffer[0], GL_STATIC_DRAW);
	// Setup the position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);
	// Setup the normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
	                      (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// Setup the texCoord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
	                      (void*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	if (useEBO) {
		// Element Buffer
		// From now on, binding to the VAO will also bind to this EBO, Letting us use
		//   glDrawElements instead of glDrawArrays for this mesh
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, elementBuffer.size() * sizeof(GLfloat),
			&elementBuffer[0], GL_STATIC_DRAW);
		// EBO's don't have attributes
		// IMPORTANT NOTE: a VAO can only have 1 EBO. The VAO uses whatever EBO was
		//   most recently bound
	}
	
	/* ----- (Optional) Unbind the Buffers & Vertex Array Object ----- */
	// This prevents further calls from accidentally modifying these objects
	// You can unbind the VBO's, since they're already registered with the VAO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// Unbind the VAO
	glBindVertexArray(0);
	// ONLY AFTER VAO is unbound, we can unbind the element buffer. You CANNOT
	//   unbind this while VAO is active, or VAO will stop recognizing it as
	//   the current EBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
