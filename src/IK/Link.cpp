#include <assert.h>
#include <iostream>

#include <glm/gtc/type_ptr.hpp>

#include "Link.h"
#include "../Mesh.h"

Link::Link(std::weak_ptr<GameEngine> engine, const std::string& name) :
	SceneObject(engine, name) {}

void Link::BeginPlay() {
	// TODO
	std::cout << objectName << " beginplay" << std::endl;
}

void Link::PhysicsUpdate() {
	std::cout << objectName << " physupdate" << std::endl;
	// Note: the J matrices are NOT updated during physicsupdate. They are the 2D J-space
	// transforms of the link, not the modelMatrix that will actually be used in rendering
}

void Link::Render(const std::shared_ptr<ShaderProgram> shader) const {
	// render children
	std::cout << objectName << " render" << std::endl;
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
