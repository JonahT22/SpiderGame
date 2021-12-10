#pragma once

#include <string>

#include <glm/glm.hpp>
#include <yaml-cpp/yaml.h>

#include "Transform.h"

///
/// Various helper functions for working with YAML files
///
/// 
/// // TODO: cleanup from header.cpp split
namespace YAML {
// Define custom parsing for glm vec3's, using starter code from yaml-cpp tutorial page
template<>
struct convert<glm::vec3> {
	// Writing vec3's to YAML files
	static Node encode(const glm::vec3& rhs);
	// Reading vec3's from YAML files
	static bool decode(const Node& node, glm::vec3& rhs);
};

// Custom parsing for glm::vec2's
template<>
struct convert<glm::vec2> {
	// Writing vec2's to YAML files
	static Node encode(const glm::vec2& rhs);
	// Reading vec2's from YAML files
	static bool decode(const Node& node, glm::vec2& rhs);
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
	static bool decode(const Node& node, Transform& rhs);
};

} //YAML


class YAMLHelper {
public:
	YAMLHelper() = default;
	~YAMLHelper() = default;

	static void PrintNodeType(const YAML::Node& node);
	// Check if a given node has a certain map field, then read it as the given type
	template <typename T>
	static T GetMapVal(const YAML::Node& map, const std::string& field_name);
	// Check if a given node has a map field with a given name, and that this field is a sequence
	static bool DoesMapHaveSequence(const YAML::Node& map_node, const std::string& sequence_name);
	// Check if a given node has a map field with a given name, and that this field is a sequence
	static bool DoesMapHaveField(const YAML::Node& map_node, const std::string& field_name);
};

template <typename T>
inline T YAMLHelper::GetMapVal(const YAML::Node& map, const std::string& field_name) {
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
