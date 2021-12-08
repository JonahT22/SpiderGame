#pragma once
#include <vector>
#include <memory>

#include <Eigen/Dense>

class Link;
class Shape;
class Program;
class MatrixStack;

class Chain
{
public:
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW;

	Chain();
	~Chain() = default;

	static void WrapAngles(Eigen::VectorXd& angles);

	void GenerateChain(size_t num_links);
	
	// Setters
	void SetLinkShape(std::shared_ptr<Shape> shape) { linkShape = shape; }
	void SetLinkPostion(double x, double y) { linkPosition << x, y; }
	void SetEndEffector(double x, double y) { endEffector << x, y, 1.0; }

	// Operations on the link angles
	void SetLinkAngles(const Eigen::VectorXd& new_angles);
	void ResetLinkAngles();
	void IncrementLinkAngles(double increment_amount);

	// Getters
	Eigen::Vector2d GetLinkPosition() const { return linkPosition; }
	Eigen::Vector3d GetEndEffector() const { return endEffector; }
	size_t GetNumLinks() const { return links.size(); }
	const Eigen::Matrix3d& GetJMatrix(size_t link_idx, size_t derivative) const;
	Eigen::VectorXd GetLinkAngles() const { return linkAngles; }
	
	void Draw(const std::shared_ptr<Program> prog, std::shared_ptr<MatrixStack> MV, const std::shared_ptr<Shape> shape);

private:
	// Iterates over each link and recalculates its transform matrices with new angles
	void UpdateLinkAngles();

	std::vector<std::shared_ptr<Link> > links;
	// Angle of each link in the chain, used to quickly get the chain's current state as a starting point for optimization
	Eigen::MatrixXd linkAngles;
	// Shape shared by each link in the chain
	std::shared_ptr<Shape> linkShape;
	// Position of every link with respect to the previous link
	Eigen::Vector2d linkPosition;
	// Transform of each link's mesh w.r.t its joint transform
	Eigen::Matrix4d meshMtx;
	// Location of the end-effector 'r', in the local space of the final link
	Eigen::Vector3d endEffector;
};

