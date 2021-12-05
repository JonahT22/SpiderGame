#pragma once

#include <string>

#include <glm/glm.hpp>
#include <yaml-cpp/yaml.h>

#include "Transform.h"

///
/// Various helper functions for working with YAML files
///
namespace YAML {
// Define custom parsing for glm vec3's, using starter code from yaml-cpp tutorial page
template<>
struct convert<glm::vec3> {
	static Node encode(const glm::vec3& rhs) {
		Node node;
		node.push_back(rhs.x);
		node.push_back(rhs.y);
		node.push_back(rhs.z);
		return node;
	}
	// Reading vec3's from YAML files
	static bool decode(const Node& node, glm::vec3& rhs) {
		if (!node.IsSequence() || node.size() != 3) {
			return false;
		}

		rhs.x = node[0].as<float>();
		rhs.y = node[1].as<float>();
		rhs.z = node[2].as<float>();
		return true;
	}
};

// Custom parsing for the Transform class
template<>
struct convert<Transform> {
	// Note: There is NO encoding function defined to write Transforms to YAML files.
	//   There is no conversion written (yet) from quat -> euler, but YAML files are
	//   written w/euler angles for readability. To be implemented later if necessary

	// Read transform from a YAML file, given in this format:
	//     location: [x,y,z]
	//     rotation: [x,y,z]
	//     scale: [x,y,z]
	static bool decode(const Node& node, Transform& rhs) {
		if (!node.IsMap()) {
			return false;
		}
		if (!node["location"] || !node["rotation"] || !node["scale"]) {
			return false;
		}

		rhs.loc = node["location"].as<glm::vec3>();
		rhs.rot = Transform::EulerToQuat(node["rotation"].as<glm::vec3>());
		rhs.scale = node["scale"].as<glm::vec3>();
		return true;
	}
};

void PrintNodeType(const YAML::Node& node) {
	switch (node.Type()) {
	case YAML::NodeType::Null:
		std::cout << "Null" << std::endl;
		break;
	case YAML::NodeType::Scalar:
		std::cout << "Scalar" << std::endl;
		break;
	case YAML::NodeType::Sequence:
		std::cout << "Sequence" << std::endl;
		break;
	case YAML::NodeType::Map:
		std::cout << "Map" << std::endl;
		break;
	case YAML::NodeType::Undefined:
	default:
		std::cout << "Undefined" << std::endl;
	}
}
}
