#include "Mesh.h"

Mesh::Mesh() :
	vertexArrayID(0),
	elementBufferID(0),
	positionBufferID(0),
	colorBufferID(0)
{

}

Mesh::~Mesh() {
	// Deallocate all of the buffers and arrays
	glDeleteVertexArrays(1, &vertexArrayID);
	glDeleteBuffers(1, &elementBufferID);
	glDeleteBuffers(1, &positionBufferID);
	glDeleteBuffers(1, &colorBufferID);
}

void Mesh::LoadMesh(const std::string& filename) {
	// TODO: later, load the mesh from a file. For now, just hardcode
	//   the mesh's points

	// Specify the positions in Normalized Device Coordinates (range -1 to 1,
	//   where 0,0,0 is the center of the world)
	// OpenGL converts this to screen-space coordinates (where 0,0 = top left)
	//   using the info provided in glviewport commands
	GLfloat positions[] = {
		// Vertex data
		0.5f,  0.5f, 0.0f,  // top right
		0.5f, -0.5f, 0.0f,  // bottom right
	   -0.5f, -0.5f, 0.0f,  // bottom left
	   -0.5f,  0.5f, 0.0f,  // top left 
	};
	positionBuffer.assign(std::begin(positions), std::end(positions));
	// Note: You could wrap the color into the same VBO as the positions, you'd just
	//  have to be careful when setting up the attribute pointers
	GLfloat colors[] = {
		1.0f, 0.0f, 0.0f,  // top right
		0.0f, 1.0f, 0.0f,  // bottom right
		0.0f, 0.0f, 1.0f,  // bottom left
		0.0f, 0.0f, 0.0f   // top left 
	};
	colorBuffer.assign(std::begin(colors), std::end(colors));
	// Texture coords, defined with (0,0) = bottom left
	GLfloat tex_coords[] = {
		1.0, 1.0,  // top right
		1.0, 0.0,  // bottom right
		0.0, 0.0,  // bottom left
		0.0, 1.0   // top left
	};
	texCoordBuffer.assign(std::begin(tex_coords), std::end(tex_coords));

	// Tells openGL which vertices to draw in what order
	//   (used by the Element Buffer Object)
	GLuint indices[] = {  //  note that we start from 0!
		0, 1, 3,   //  first triangle
		1, 2, 3    //  second triangle
	};
	elementBuffer.assign(std::begin(indices), std::end(indices));

	// Once the mesh data is finished loading, set up the required buffers &
	//   vertex array with OpenGL
	SetupVertexArray();
	// TODO: Should this be a separate function, or can this be part of loadmesh?
}

void Mesh::Render() {
	// Start with the VAO, which has all of the buffer/attribute settings
	glBindVertexArray(vertexArrayID);
	/* This code is for if we want to draw straight from the vertex buffer
	  glDrawArrays(GL_TRIANGLES, 0, 3);
	  // ^ 1: the primitive type that we want to draw
	  //   2: The starting index of the vertex array that we want to draw
	  //   3: How many vertices we want to draw
	*/
	// Instead, let's draw from the element buffer that's bound to our VAO
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	// ^ 1: the primitive type (just like the VBO DrawArrays version)
	//   2: # of elements to draw (we have 6 indices, so draw 6 vertices
	//   3: the type of the indices
	//   4: offset or array ref, we don't need to worry abt it now

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
	//   Note: you could create all 3 at once, but for readability I'll split it up
	// Create VBOs - Big buffers for sending a bunch of data to the GPU at once
	glGenBuffers(1, &positionBufferID);
	glGenBuffers(1, &colorBufferID);
	glGenBuffers(1, &texCoordBufferID);
	// Create the EBO - a way to specify both the vertices, and the order to draw them.
	//   This lets you do things like draw a rectangle by only specifying 4 points
	//   (otherwise, you'd have to define two triangles: 6 points,
	//   2 of them identical)
	glGenBuffers(1, &elementBufferID);

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

	// Position Data
	glBindBuffer(GL_ARRAY_BUFFER, positionBufferID);
	glBufferData(GL_ARRAY_BUFFER, positionBuffer.size() * sizeof(GLfloat), 
	             &positionBuffer[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// If there was more data attached to the same buffer, we'd add attribute
	//   pointers here with a different offset & stride
	// ex: glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

	// Color Data
	glBindBuffer(GL_ARRAY_BUFFER, colorBufferID);
	glBufferData(GL_ARRAY_BUFFER, colorBuffer.size() * sizeof(GLfloat),
	             &colorBuffer[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);

	// Texture Coordinate Data
	glBindBuffer(GL_ARRAY_BUFFER, texCoordBufferID);
	glBufferData(GL_ARRAY_BUFFER, texCoordBuffer.size() * sizeof(GLfloat),
	             &texCoordBuffer[0], GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(2);

	// Element Buffer
	// From now on, binding to the VAO will also bind to this EBO, Letting us use
	//   glDrawElements instead of glDrawArrays for this mesh
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, elementBuffer.size() * sizeof(GLfloat),
	             &elementBuffer[0], GL_STATIC_DRAW);
	// EBO's don't have attributes
	// IMPORTANT NOTE: a VAO can only have 1 EBO. The VAO uses whatever EBO was
	//   most recently bound

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
