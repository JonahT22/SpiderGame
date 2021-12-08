#include <assert.h>
#include <iostream>

#define GLEW_STATIC
#include <GL/glew.h>

#include <glm/gtc/type_ptr.hpp>

#include "Link.h"
#include "Shape.h"
#include "MatrixStack.h"
#include "Program.h"

using namespace std;
using namespace Eigen;

Link::Link() :
	parent(NULL),
	child(NULL),
	depth(0),
	position(0.0, 0.0),
	angle(0.0),
	meshMat(Matrix4d::Identity())
{
	
}

Link::~Link()
{
	
}

void Link::addChild(shared_ptr<Link> child)
{
	child->parent = shared_from_this();
	child->depth = depth + 1;
	this->child = child;
}

void Link::setAngle(double a)
{
	angle = a;
	
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

void Link::draw(const shared_ptr<Program> prog, shared_ptr<MatrixStack> MV, const shared_ptr<Shape> shape) const
{
	assert(prog);
	assert(MV);
	assert(shape);
	
	MV->pushMatrix();
	
	// Apply joint frame transformations
	MV->translate(Eigen::Vector3d(position[0], position[1], 0));
	MV->rotate(angle, Eigen::Vector3d(0, 0, 1));

	// Apply mesh frame transformations
	MV->pushMatrix();
	MV->multMatrix(meshMat);

	// Send matrix stack to GPU and draw the link
	glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, glm::value_ptr(MV->topMatrix()));
	shape->draw();

	// Pop the mesh frame
	MV->popMatrix();

	// Draw children in the joint frame
	if (child != nullptr) {
		child->draw(prog, MV, shape);
	}
	
	// Pop the joint frame
	MV->popMatrix();
}
