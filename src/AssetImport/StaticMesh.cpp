#include <vector>
#include <string>

#include <glad/glad.h>

#include "StaticMesh.h"
#include "Texture.h"
#include "../Rendering/ShaderProgram.h"

using T = Texture::TextureType;

StaticMesh::StaticMesh(std::vector<Vertex>& vertices, 
                       std::vector<GLuint>& indices,
                       std::vector<Texture>& textures) {
	vertexBuffer = std::move(vertices);
    elementBuffer = std::move(indices);
    textureList = std::move(textures);
	// Only allocate and draw using the ebo if indices are provided
	useEBO = (elementBuffer.size() > 0);
}

StaticMesh::~StaticMesh() {
    // Deallocate all of the buffers and arrays
    glDeleteVertexArrays(1, &vertexArrayID);
    glDeleteBuffers(1, &vertexBufferID);
    glDeleteBuffers(1, &elementBufferID);
}

void StaticMesh::Render(const std::shared_ptr<ShaderProgram> shader) const {
	// Bind the textures for this object. Each object can have multiple textures
	//   of several types. Bind them to texture units, and set the uniforms for the
	//   texture samplers in the shader to the corresponding texture units
	// Assume shaders use the naming convention:
	//   textureDiffuse0
	//   textureDiffuse1
	//   textureSpecular0, etc...

	// Keep track of how many textures of each type have been bound
	constexpr GLuint num_tex_types = static_cast<GLuint>(Texture::TextureType::ENUM_END);
	std::vector<GLuint> tex_counts(num_tex_types, 0);
	// Try binding each texture and send the texture's unit to the shader
	for (size_t i = 0; i < textureList.size(); ++i) {
		// Bind textures to units 0, 1, 2, ...
		glActiveTexture(GL_TEXTURE0 + i);

		// Get a string representing this texture's type
		const Texture::TextureType tex_type = textureList.at(i).GetType();
		const std::string type_string = Texture::TypeToString(tex_type);
		// Find the 'number' of this texture (i.e. diffuse_0 vs diffuse_1)
		const GLuint tex_type_idx = static_cast<GLuint>(tex_type);
		// Get the texture's number, THEN increment the counter for this texture type
		const std::string num_string = std::to_string(tex_counts.at(tex_type_idx)++);
		
		// Set the texture unit value on the shader
		// TODO: remove print
		std::cout << "Setting texture name: " << "texture" + type_string + num_string;
		std::cout << " to unit number " << i << std::endl;
		shader->SetIntUniform("texture" + type_string + num_string, i);
	}
	glActiveTexture(GL_TEXTURE0);

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

void StaticMesh::SetupVertexArray() {
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
	glBufferData(GL_ARRAY_BUFFER, vertexBuffer.size() * sizeof(Vertex),
		&vertexBuffer[0], GL_STATIC_DRAW);
	// Setup the position attribute. Use the offsetof() macro for getting vertex offsets
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		(void*)offsetof(Vertex, position));
	// Setup the normal attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		(void*)offsetof(Vertex, normal));
	// Setup the texCoord attribute
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		(void*)offsetof(Vertex, texCoord));

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
