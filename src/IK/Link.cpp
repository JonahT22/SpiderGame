#include <assert.h>
#include <iostream>
#include <memory>
#include <string>

#include <glm/gtc/type_ptr.hpp>

#include "Link.h"
#include "../Mesh.h"

Link::Link(std::weak_ptr<GameEngine> engine, const std::string& name) :
	SceneObject(engine, name) {}

void Link::BeginPlay() {
	if (SceneObject::enable_shared_from_this::weak_from_this().expired()) {
		std::cout << "ERROR: link " << objectName << " has expired weak_from_this!" << std::endl;
	}
	// Create the mesh that this link will use
	linkMesh = std::make_shared<Mesh>(engineRef, objectName + "_mesh");
	linkMesh->GenerateCubeMesh();
	// TODO: remove hardcoding
	linkMesh->LoadTexture("resources/textures/awesomeface.png");
	linkMesh->SetRelativeLocation(glm::vec3(0.0f, 0.0f, 0.0f));
	linkMesh->SetRelativeScale(glm::vec3(0.1f, 0.1f, 0.1f));
	AddChildObject(linkMesh);
	// Manually pass BeginPlay to the mesh that this link controls, since it's not
	//   managed by the scene
	linkMesh->BeginPlay();
}

void Link::PhysicsUpdate() {
	// Note: the J matrices are NOT updated during physicsupdate. They are the 2D J-space
	// transforms of the link, not the modelMatrix that will actually be used in rendering
	
	SceneObject::PhysicsUpdate();
}

void Link::Render(const std::shared_ptr<ShaderProgram> shader) const {
	// Manually render this link's mesh, since it's not controlled by the scene
	linkMesh->Render(shader);
}

void Link::SetAngle(double a)
{	
	// Hardcoded link offset
	glm::vec2 position(1.0f, 0.0f);

	// Recalculate the J matrices for this link
	double cosT = cos(a);
	double sinT = sin(a);
	J <<
		cosT, -1.0 * sinT, position[0],
		sinT, cosT, position[1],
		0, 0, 1;
	// Note: translation coords are 0 for J' and J", because
	//   R matrix has homogeneous coord of 0 (which nullifies
	//   the translation matrix)
	JPrime <<
		-1.0 * sinT, -1.0 * cosT, 0,
		cosT, -1.0 * sinT, 0,
		0, 0, 0;
	J2Prime <<
		-1.0 * cosT, sinT, 0,
		-1.0 * sinT, -1.0 * cosT, 0,
		0, 0, 0;
}
