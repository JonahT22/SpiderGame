#pragma once
#ifndef Link_H
#define Link_H

#include <memory>

#include <Eigen/Dense>

class Shape;
class MatrixStack;
class Program;

// Note: enable_shared_from_this allows creating shared ptrs for other objects to use from
// within this class while keeping proper reference counts
class Link : public std::enable_shared_from_this<Link>
{
public:
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
	
	Link();
	virtual ~Link();

	void addChild(std::shared_ptr<Link> child);
	std::shared_ptr<Link> getChild() const { return child; }
	std::shared_ptr<Link> getParent() const { return parent; }
	int getDepth() const { return depth; }

	const Eigen::Matrix3d& GetJ() const { return J; }
	const Eigen::Matrix3d& GetJPrime() const { return JPrime; }
	const Eigen::Matrix3d& GetJ2Prime() const { return J2Prime; }

	void setPosition(Eigen::Vector2d pos) { position = pos; }
	const Eigen::Vector2d &getPosition() const { return position; }

	void setAngle(double a);
	double getAngle() const { return angle; }

	void setMeshMatrix(const Eigen::Matrix4d &M) { meshMat = M; }

	void draw(const std::shared_ptr<Program> prog, std::shared_ptr<MatrixStack> MV, const std::shared_ptr<Shape> shape) const;
	
private:
	std::shared_ptr<Link> parent;
	std::shared_ptr<Link> child;
	int depth;
	Eigen::Vector2d position;
	double angle;
	Eigen::Matrix4d meshMat;

	// Store the 'J' matrices for this link, which are equal to TR(theta), TR'(theta), and TR"(theta)
	Eigen::Matrix3d J;
	Eigen::Matrix3d JPrime;
	Eigen::Matrix3d J2Prime;
};

#endif
