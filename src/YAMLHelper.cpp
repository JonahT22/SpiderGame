#include "YAMLHelper.h"

#include <string>

#include <glm/glm.hpp>
#include <yaml-cpp/yaml.h>

#include "Transform.h"
// TODO: cleanup from header/cpp split
namespace YAML {
// Define custom parsing for glm vec3's, using starter code from yaml-cpp tutorial page
Node convert<glm::vec3>::encode(const glm::vec3& rhs) {
	Node node;
	node.push_back(rhs.x);
	node.push_back(rhs.y);
	node.push_back(rhs.z);
	return node;
}

bool convert<glm::vec3>::decode(const Node& node, glm::vec3& rhs) {
	if (!node.IsSequence() || node.size() != 3) {
		return false;
	}

	rhs.x = node[0].as<float>();
	rhs.y = node[1].as<float>();
	rhs.z = node[2].as<float>();
	return true;
}

// Custom parsing for glm::vec2's
Node convert<glm::vec2>::encode(const glm::vec2& rhs) {
	Node node;
	node.push_back(rhs.x);
	node.push_back(rhs.y);
	return node;
}

bool convert<glm::vec2>::decode(const Node& node, glm::vec2& rhs) {
	if (!node.IsSequence() || node.size() != 2) {
		return false;
	}

	rhs.x = node[0].as<float>();
	rhs.y = node[1].as<float>();
	return true;
}

bool convert<Transform>::decode(const Node& node, Transform& rhs) {
	if (!node.IsMap()) {
		return false;
	}
	if (!node["location"] || !node["rotation"] || !node["scale"]) {
		return false;
	}

	rhs.loc = node["location"].as<glm::vec3>();
	glm::vec3 degrees = node["rotation"].as<glm::vec3>();
	glm::vec3 radians = glm::radians(degrees);
	rhs.rot = Transform::EulerToQuat(radians);
	rhs.scale = node["scale"].as<glm::vec3>();
	return true;
}

} // YAML

void YAMLHelper::PrintNodeType(const YAML::Node& node) {
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

bool YAMLHelper::DoesMapHaveSequence(const YAML::Node& map_node, const std::string& sequence_name) {
	// TODO: test this thoroughly
	if (!map_node.IsMap()) {
		std::cerr << "ERROR: Attempted to get a sequence from a map, but the provided";
		std::cerr << " node is not a map!" << std::endl;
		return false;
	}
	else if (!map_node[sequence_name]) {
		std::cerr << "ERROR: YAML map has no \"" << sequence_name << "\" field!" << std::endl;
		return false;
	}
	else if (!map_node[sequence_name].IsSequence()) {
		std::cerr << "ERROR: Attempted to read " << sequence_name << " node from YAML map,";
		std::cerr << "but the " << sequence_name << " node is not a YAML::Sequence!" << std::endl;
		return false;
	}
	else {
		return true;
	}
}

bool YAMLHelper::DoesMapHaveField(const YAML::Node& map_node, const std::string& field_name) {
	// TODO: test this thoroughly
	if (!map_node.IsMap()) {
		std::cerr << "ERROR: Attempted to get a field from a map, but the provided";
		std::cerr << " node is not a map!" << std::endl;
		return false;
	}
	if (map_node[field_name]) {
		return true;
	}
	else return false;
}