#include <vector>
#include <string>

#include <glad/glad.h>

#include "StaticMesh.h"
#include "Texture.h"
#include "../Rendering/ShaderProgram.h"

using T = Texture::TextureType;

StaticMesh::StaticMesh(std::vector<Vertex>& vertices, 
                       std::vector<GLuint>& indices,
                       std::vector<Texture>& textures) :
	vertexArrayID(0),
	vertexBufferID(0),
	elementBufferID(0) {
	vertexBuffer = std::move(vertices);
    elementBuffer = std::move(indices);
    textureList = std::move(textures);
	// Only allocate and draw using the ebo if indices are provided
	useEBO = (elementBuffer.size() > 0);
	SetupVertexArray();
}

StaticMesh::~StaticMesh() {
    // Deallocate all of the buffers and arrays
    glDeleteVertexArrays(1, &vertexArrayID);
    glDeleteBuffers(1, &vertexBufferID);
    glDeleteBuffers(1, &elementBufferID);
}

void StaticMesh::Render(const std::shared_ptr<ShaderProgram> shader) const {
	/* ----- Bind texture data ----- */
	// Each object can have multiple textures
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
		textureList.at(i).Bind(i);
		// TODO: does this work without using texture.bind()?
		//glActiveTexture(GL_TEXTURE0 + i);

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

	/* ----- Bind vertex data & draw the mesh ----- */
	// Load this mesh's buffer/attribute settings
	GLint vaidx;
	glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &vaidx);
	glBindVertexArray(vertexArrayID);
	glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &vaidx);
	// Draw the mesh using either indexed EBO drawing or raw VBO drawing
	if (useEBO) {
		GLint ebufidx;
		GLint vbufidx;
		glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &ebufidx);
		glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &vbufidx);
		// TODO: shouldn't need to re-bind here
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferID);

		glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &ebufidx);
		glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &vbufidx);

		// Draw the mesh using indexed drawing & the element buffer
		glDrawElements(GL_TRIANGLES, elementBuffer.size(), GL_UNSIGNED_INT, 0);
		// ^ 1: the primitive type (just like the VBO DrawArrays version)
		//   2: # of elements to draw
		//   3: the type of the indices
		//   4: offset or array ref, we don't need to worry abt it now
	}
	else {
		// TODO: I think Assimp automatically makes EVERYTHING use indexed drawing. Does
		//   this ever get called? If so, is that /8 correct?

		// Draw straight from the VBO (w/out indexed drawing)
		glDrawArrays(GL_TRIANGLES, 0, vertexBuffer.size() / 8);
		// ^ 1: the primitive type that we want to draw
		//   2: The starting index of the vertex array that we want to draw
		//   3: How many vertices we want to draw
	}
}

void StaticMesh::AddTexture(const Texture& new_tex) {
	textureList.emplace_back(new_tex);
}

void StaticMesh::ClearTextures() {
	textureList.clear();
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

	// TODO: remove
	GLint ebufidx;
	GLint vbufidx;
	glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &ebufidx);
	glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &vbufidx);

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


	// TODO: remove
	glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &ebufidx);
	glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &vbufidx);
	// Try re-binding the vertex array to see if the values hold
	glBindVertexArray(vertexArrayID);
	glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &ebufidx);
	glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &vbufidx);
	std::cout << ebufidx << ", " << vbufidx << std::endl;
}
