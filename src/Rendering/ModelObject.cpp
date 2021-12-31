#include <cassert>
#include <iostream>

#include "ModelObject.h"
#include "ShaderProgram.h"

ModelObject::ModelObject(std::weak_ptr<GameEngine> engine, const std::string& name) :
	SceneObject(engine, name) {

}

void ModelObject::BeginPlay() {}

void ModelObject::LoadMesh(const std::string& filename, const bool use_EBO) {
	std::cerr << "ERROR: MeshObject::LoadMesh() is not yet implemented!" << std::endl;
	assert(0);

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

void ModelObject::GenerateCubeMesh(const float tex_mult_u, const float tex_mult_v) {
	useEBO = false;
	// Specify the positions in Normalized Device Coordinates (range -1 to 1,
	//   where 0,0,0 is the center of the world)
	// OpenGL converts this to screen-space coordinates (where 0,0 = top left)
	//   using the info provided in glviewport commands
	GLfloat positions[] = {
		// Vertex data for a cube
		// posX, posY, posZ,  norX,  norY,  norZ, texU, texV,
		// Back
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f * tex_mult_u, 0.0f * tex_mult_v,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f * tex_mult_u, 0.0f * tex_mult_v,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f * tex_mult_u, 1.0f * tex_mult_v,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f * tex_mult_u, 1.0f * tex_mult_v,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f * tex_mult_u, 1.0f * tex_mult_v,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f * tex_mult_u, 0.0f * tex_mult_v,
		// Front
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f * tex_mult_u, 0.0f * tex_mult_v,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f * tex_mult_u, 0.0f * tex_mult_v,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f * tex_mult_u, 1.0f * tex_mult_v,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f * tex_mult_u, 1.0f * tex_mult_v,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f * tex_mult_u, 1.0f * tex_mult_v,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f * tex_mult_u, 0.0f * tex_mult_v,
		// Left
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 1.0f * tex_mult_u, 0.0f * tex_mult_v,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 1.0f * tex_mult_u, 1.0f * tex_mult_v,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f * tex_mult_u, 1.0f * tex_mult_v,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f * tex_mult_u, 1.0f * tex_mult_v,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 0.0f * tex_mult_u, 0.0f * tex_mult_v,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 1.0f * tex_mult_u, 0.0f * tex_mult_v,
		// Right
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 1.0f * tex_mult_u, 0.0f * tex_mult_v,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 1.0f * tex_mult_u, 1.0f * tex_mult_v,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 0.0f * tex_mult_u, 1.0f * tex_mult_v,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 0.0f * tex_mult_u, 1.0f * tex_mult_v,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 0.0f * tex_mult_u, 0.0f * tex_mult_v,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 1.0f * tex_mult_u, 0.0f * tex_mult_v,
		// Bottom
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 0.0f * tex_mult_u, 1.0f * tex_mult_v,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 1.0f * tex_mult_u, 1.0f * tex_mult_v,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 1.0f * tex_mult_u, 0.0f * tex_mult_v,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 1.0f * tex_mult_u, 0.0f * tex_mult_v,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 0.0f * tex_mult_u, 0.0f * tex_mult_v,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 0.0f * tex_mult_u, 1.0f * tex_mult_v,
		// Top
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 0.0f * tex_mult_u, 1.0f * tex_mult_v,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 1.0f * tex_mult_u, 1.0f * tex_mult_v,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 1.0f * tex_mult_u, 0.0f * tex_mult_v,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 1.0f * tex_mult_u, 0.0f * tex_mult_v,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 0.0f * tex_mult_u, 0.0f * tex_mult_v,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 0.0f * tex_mult_u, 1.0f * tex_mult_v
	};
	vertexBuffer.assign(std::begin(positions), std::end(positions));

	SetupVertexArray();
}

void ModelObject::LoadTexture(const std::string& filename) {
	texture.LoadFromFile(filename);
}

void ModelObject::Render(const std::shared_ptr<ShaderProgram> shader) const {
	// Call the parent's render function, which checks if the shader is bound and sends
	//   the model/invT matrices
	SceneObject::Render(shader);

	// Set the mesh's texture to texture unit 0 (default)
	if (texture.IsLoaded()) {
		texture.Bind();
	}
	
}

