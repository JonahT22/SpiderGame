#include <assert.h>
#include <iostream>
#include <memory>
#include <string>

#include <glm/gtc/type_ptr.hpp>

#include "Link.h"
#include "../Rendering/MeshObject.h"
#include "../Utils/Transform.h"

Link::Link(std::weak_ptr<GameEngine> engine, const std::string& name, const float length) :
	SceneObject(engine, name), linkLength(length) {}

void Link::BeginPlay() {
	// Create the mesh that this link will use
	linkMesh = std::make_shared<ModelObject>(engineRef, objectName + "_mesh");
	linkMesh->GenerateCubeMesh(linkLength, 0.1f);
	// TODO: remove hardcoding
	linkMesh->LoadTexture("resources/textures/fabric.jpg");
	linkMesh->SetRelativeLocation(glm::vec3(linkLength / 2.0f, 0.0f, 0.0f));
	linkMesh->SetRelativeScale(glm::vec3(linkLength, 0.1f, 0.1f));
	AddChildObject(linkMesh);
	// Manually pass BeginPlay to the mesh that this link controls, since it's not
	//   managed by the scene
	linkMesh->BeginPlay();
}

void Link::PhysicsUpdate(const float delta_time) {
	// Note: the J matrices are NOT updated during physicsupdate. They are the 2D J-space
	// transforms of the link, not the modelMatrix that will actually be used in rendering
	
	SceneObject::PhysicsUpdate(delta_time);
}

void Link::Render(const std::shared_ptr<ShaderProgram> shader) const {
	// Manually render this link's mesh, since it's not controlled by the scene
	linkMesh->Render(shader);
}

void Link::SetLinkAngle(double a) {	
	// Apply new angle to this link's local transform
	// TODO: check if a rotation along z is correct
	rootTransform.rot = Transform::EulerToQuat(glm::vec3(0.0, 0.0, a));
	MarkPhysicsDirty();

	// Recalculate the J matrices for this link
	double cosT = cos(a);
	double sinT = sin(a);
	J <<
		cosT, -1.0 * sinT, rootTransform.loc.x,
		sinT, cosT, rootTransform.loc.y,
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
