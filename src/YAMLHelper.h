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
	// Writing vec3's to YAML files
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

// Custom parsing for glm::vec2's
template<>
struct convert<glm::vec2> {
	// Writing vec2's to YAML files
	static Node encode(const glm::vec2& rhs) {
		Node node;
		node.push_back(rhs.x);
		node.push_back(rhs.y);
		return node;
	}
	// Reading vec2's from YAML files
	static bool decode(const Node& node, glm::vec2& rhs) {
		if (!node.IsSequence() || node.size() != 2) {
			return false;
		}

		rhs.x = node[0].as<float>();
		rhs.y = node[1].as<float>();
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
		glm::vec3 degrees = node["rotation"].as<glm::vec3>();
		glm::vec3 radians = glm::radians(degrees);
		rhs.rot = Transform::EulerToQuat(radians);
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

// Check if a given node has a certain map field, then read it as the given type
template <typename T>
T GetMapVal(const YAML::Node& map, const std::string& field_name) {
	if (!map.IsMap()) {
		std::cerr << "ERROR: Tried to get a value from a map, but the provided node";
		std::cerr << " is not a map!" << std::endl;
		return T();
	}
	
	if (!map[field_name]) {
		std::cerr << "ERROR: tried to get field \"" << field_name << "\" from a";
		std::cerr << " YAML map, but the field does not exist!" << std::endl;
		return T();
	}

	return map[field_name].as<T>();
}

// Check if a given node has a map field with a given name, and that this field is a sequence
bool DoesMapHaveSequence(const YAML::Node& map_node, const std::string& sequence_name) {
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
}
