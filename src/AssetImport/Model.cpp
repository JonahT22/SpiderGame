#include <iostream>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Model.h"
#include "Texture.h"

void Model::LoadFromFile(const std::string& filename, bool load_textures) {
	Assimp::Importer importer;
	// Load the model's file into an Assimp scene (different than the Scene class)
	// Read the file with some aiPostProcessSteps flags (see assimp->postprocess.h)
	//   - Triangulate: make sure the file is read as a triangle mesh
	//   - OptimizeMeshes: try to combine meshes to reduce the # of draw calls
	//   - TODO: JoinIdenticalVertices might be a good one too
	const aiScene* scene = importer.ReadFile(filename, aiProcess_Triangulate |
		aiProcess_FlipUVs | aiProcess_OptimizeMeshes);
	// Check if the scene loaded properly
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cout << "ERROR while loading model with Assimp: ";
		std::cout << importer.GetErrorString() << std::endl;
		return;
	}
	modelDir = filename.substr(0, filename.find_last_of('/'));
	ProcessNode(scene->mRootNode, scene, load_textures);
}

void Model::OverrideTextures(const Texture& override_texture) {
	// Clear any loaded textures and replace with the override texture
	for (auto& mesh : meshList) {
		mesh.ClearTextures();
		mesh.AddTexture(override_texture);
	}
}

void Model::Render(const std::shared_ptr<ShaderProgram>& shader) const {
	for (auto& mesh : meshList) {
		mesh.Render(shader);
	}
}

void Model::ProcessNode(aiNode* node, const aiScene* scene, bool load_textures) {
	// Assimp scenes have a heirarchy of nodes, and each node can have multiple meshes
	// Get the meshes attached to this node
	for (size_t i = 0; i < node->mNumMeshes; ++i) {
		aiMesh* new_mesh = scene->mMeshes[node->mMeshes[i]];
		meshList.emplace_back(ProcessMesh(new_mesh, scene, load_textures));
	}
	// Process the child nodes recursively
	for (size_t i = 0; i < node->mNumChildren; ++i) {
		ProcessNode(node->mChildren[i], scene, load_textures);
	}
	// Note: Assimp can read nodes/meshes in parent-child relationships, but our model
	//   class doesn't store these relationships. If an assimp scene is read that has
	//   parents & children, the Model class will need to be updated to account for them
}

StaticMesh Model::ProcessMesh(aiMesh* mesh, const aiScene* scene, bool load_textures) {
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<Texture> textures;

	/* ----- Process vertices ----- */
	for (size_t i = 0; i < mesh->mNumVertices; ++i) {
		Vertex new_vertex;
		// Position
		new_vertex.position.x = mesh->mVertices[i].x;
		new_vertex.position.y = mesh->mVertices[i].y;
		new_vertex.position.z = mesh->mVertices[i].z;
		// Normal
		new_vertex.normal.x = mesh->mNormals[i].x;
		new_vertex.normal.y = mesh->mNormals[i].y;
		new_vertex.normal.z = mesh->mNormals[i].z;
		// Check if the mesh has tex coords
		if (mesh->mTextureCoords[0]) {
			// Assimp meshes may have multiple sets of tex coords, but just
			//   grab the first set
			new_vertex.texCoord.x = mesh->mTextureCoords[0][i].x;
			new_vertex.texCoord.y = mesh->mTextureCoords[0][i].y;
		}
		else {
			new_vertex.texCoord = glm::vec2(0.0f, 0.0f);
		}
		vertices.emplace_back(new_vertex);
	}

	/* ----- Process indices ----- */
	// Assimp always stores each "face" (triangle) as a set of indices, so it's
	//   already set up to use indexed drawing
	for (size_t i = 0; i < mesh->mNumFaces; ++i) {
		aiFace face = mesh->mFaces[i];
		for (size_t j = 0; j < face.mNumIndices; ++j) {
			indices.emplace_back(face.mIndices[j]);
		}
	}

	/* ----- Process textures ----- */
	if (mesh->mMaterialIndex >= 0) {
		// Get the material from the scene
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		if (load_textures) {
			// An assimp material contains many properties, including lists of 
			//   textures for diffuse, specular, emmissive, etc (see material.h)
			// Load diffuse & specular textures
			LoadTexturesFromMaterial(material, aiTextureType_DIFFUSE,
				Texture::TextureType::DIFFUSE, textures);
			LoadTexturesFromMaterial(material, aiTextureType_SPECULAR,
				Texture::TextureType::SPECULAR, textures);
		}
	}
	// TODO: get a default texture from the scene if there is no material, or if there
	//   are no textures on the material

	return StaticMesh(vertices, indices, textures);
}

void Model::LoadTexturesFromMaterial(aiMaterial* material, aiTextureType ai_tex_type,
	Texture::TextureType custom_tex_type, std::vector<Texture>& tex_list) {
	for (size_t i = 0; i < material->GetTextureCount(ai_tex_type); ++i) {
		// Construct a texture object from the path in the material
		aiString local_tex_path;
		material->GetTexture(ai_tex_type, i, &local_tex_path);
		std::string path = modelDir + "/" + std::string(local_tex_path.C_Str());
		tex_list.emplace_back(path, custom_tex_type);
		// TODO: Get texture import info (wrapping, wireframe, 2 sided,
		//     etc) from the material (see assimp material.h)
		// TODO: have the scene hold a list of all imported textures (and models),
		//   keep a hash map from paths -> indices in a master list,
		//   and copy the entry from the list instead of loading a new texture from
		//   scratch if it matches the path of an already-loaded texture
	}
}
