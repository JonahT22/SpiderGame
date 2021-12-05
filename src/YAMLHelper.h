#pragma once

#include <glm/glm.hpp>
#include <yaml-cpp/yaml.h>

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
