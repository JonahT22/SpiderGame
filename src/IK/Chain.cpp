#include <iostream>

#include "Chain.h"
#include "Link.h"

using namespace std;

Chain::Chain()
{
	endEffector << 1.0, 0.0, 1.0;
	linkPosition << 1.0, 0.0;
	meshMtx = Eigen::Matrix4d::Identity();
	// The joint location is at the end of each link, not the middle
	meshMtx.col(3) = Eigen::Vector4d(0.5, 0, 0, 1);
}

void Chain::WrapAngles(Eigen::VectorXd& angles)
{
	// Wrap each angle from -pi to pi
	size_t num_angles = angles.rows();
	double pi = 3.1415926535;
	for (size_t i = 0; i < num_angles; ++i) {
		while (angles(i) > pi) {
			angles(i) -= 2.0 * pi;
		}
		while (angles(i) < -1.0 * pi) {
			angles(i) += 2.0 * pi;
		}
	}
}

void Chain::GenerateChain(size_t num_links)
{
	for (size_t i = 0; i < num_links; ++i) {
		links.push_back(std::make_shared<Link>());
		links.at(i)->setMeshMatrix(meshMtx);
		if (i != 0) {
			links.at(i - 1)->addChild(links.at(i));
			links.at(i)->setPosition(linkPosition);
		}
	}
	// Initialize all link angles as 0
	linkAngles = Eigen::VectorXd::Zero(num_links);
	UpdateLinkAngles();
}

void Chain::SetLinkAngles(const Eigen::VectorXd& new_angles)
{
	assert(new_angles.rows() == linkAngles.rows());
	// Copy the new link angles into this class' angle list (for future reference)
	linkAngles = new_angles;
	UpdateLinkAngles();
}

void Chain::ResetLinkAngles()
{
	linkAngles = Eigen::VectorXd::Zero(links.size());
	UpdateLinkAngles();
}

void Chain::IncrementLinkAngles(double increment_amount)
{
	for (size_t i = 0; i < links.size(); ++i) {
		linkAngles(i) += increment_amount;
	}
	UpdateLinkAngles();
}

const Eigen::Matrix3d& Chain::GetJMatrix(size_t link_idx, size_t derivative) const
{
	assert(derivative <= 2);
	if (derivative == 0) {
		return links.at(link_idx)->GetJ();
	}
	else if (derivative == 1) {
		return links.at(link_idx)->GetJPrime();
	}
	else if (derivative == 2) {
		return links.at(link_idx)->GetJ2Prime();
	}
	else {
		std::cerr << "Error: Accessing invalid link matrix derivative!" << std::endl;
		return Eigen::Matrix3d::Identity();
	}
}

void Chain::Draw(const std::shared_ptr<Program> prog, std::shared_ptr<MatrixStack> MV, const std::shared_ptr<Shape> shape)
{
	links.at(0)->draw(prog, MV, shape);
}

// PRIVATE FUNCTIONS

void Chain::UpdateLinkAngles()
{
	for (size_t i = 0; i < links.size(); ++i) {
		links.at(i)->setAngle(linkAngles(i));
	}
}