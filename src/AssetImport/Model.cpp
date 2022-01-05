#include <iostream>
#include <memory>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glad/glad.h>
#include <glm/glm.hpp>

#include "../Rendering/Scene.h"
#include "Model.h"
#include "Texture.h"

Model::Model(const std::string& filename, std::weak_ptr<Scene> scene_ref) {
	Assimp::Importer importer;
	// Load the model's file into an Assimp scene (different than the Scene class)
	// Read the file with some aiPostProcessSteps flags (see assimp->postprocess.h)
	//   - Triangulate: make sure the file is read as a triangle mesh
	//   - OptimizeMeshes: try to combine meshes to reduce the # of draw calls
	//   - JoinIdenticalVertices: Reduce number of vertices for indexed drawing
	const aiScene* ai_scene = importer.ReadFile(filename, aiProcess_Triangulate |
		aiProcess_FlipUVs | aiProcess_OptimizeMeshes | aiProcess_JoinIdenticalVertices);
	// Check if the scene loaded properly
	if (!ai_scene || ai_scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !ai_scene->mRootNode) {
		std::cout << "ERROR while loading model with Assimp: ";
		std::cout << importer.GetErrorString() << std::endl;
		return;
	}
	modelDir = filename.substr(0, filename.find_last_of('/'));
	// Iterate through the scene and load the meshes & textures
	ProcessNode(ai_scene->mRootNode, ai_scene, scene_ref);
}

void Model::Render(const std::shared_ptr<ShaderProgram>& shader,
                   const std::weak_ptr<Texture> tex_override) const {
	for (auto& mesh : meshList) {
		mesh->Render(shader, tex_override);
	}
}

void Model::ProcessNode(aiNode* node, const aiScene* scene,
	std::weak_ptr<Scene> scene_ref) {
	// Assimp scenes have a heirarchy of nodes, and each node can have multiple meshes
	// Get the meshes attached to this node
	for (size_t i = 0; i < node->mNumMeshes; ++i) {
		aiMesh* new_mesh = scene->mMeshes[node->mMeshes[i]];
		ProcessMesh(new_mesh, scene, scene_ref);
	}
	// Process the child nodes recursively
	for (size_t i = 0; i < node->mNumChildren; ++i) {
		ProcessNode(node->mChildren[i], scene, scene_ref);
	}
	// Note: Assimp can read nodes/meshes in parent-child relationships, but our model
	//   class doesn't store these relationships. If an assimp scene is read that has
	//   parents & children, the Model class will need to be updated to account for them
}

void Model::ProcessMesh(aiMesh* mesh, const aiScene* scene,
	std::weak_ptr<Scene> scene_ref) {
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<std::weak_ptr<Texture> > textures;

	/* ----- Process vertices ----- */
	for (size_t i = 0; i < mesh->mNumVertices; ++i) {
		Vertex new_vertex;
		// Positions
		new_vertex.position.x = mesh->mVertices[i].x;
		new_vertex.position.y = mesh->mVertices[i].y;
		new_vertex.position.z = mesh->mVertices[i].z;
		// Normals
		if (mesh->HasNormals()) {
			new_vertex.normal.x = mesh->mNormals[i].x;
			new_vertex.normal.y = mesh->mNormals[i].y;
			new_vertex.normal.z = mesh->mNormals[i].z;
		}
		// Texture coords
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
		// An assimp material contains many properties, including lists of 
		//   textures for diffuse, specular, emissive, etc (see material.h)
		// Load diffuse & specular textures
		LoadTexturesFromMaterial(material, aiTextureType_DIFFUSE,
			Texture::TextureType::DIFFUSE, textures, scene_ref);
		LoadTexturesFromMaterial(material, aiTextureType_SPECULAR,
			Texture::TextureType::SPECULAR, textures, scene_ref);
	}

	meshList.emplace_back(std::make_shared<StaticMesh>(vertices, indices, textures));
}

void Model::LoadTexturesFromMaterial(aiMaterial* material,
	aiTextureType ai_tex_type,
	Texture::TextureType custom_tex_type,
	std::vector<std::weak_ptr<Texture> >& tex_list,
	std::weak_ptr<Scene> scene_ref) {

	// Keep track of how many materials of each type have been encountered
	

	for (size_t i = 0; i < material->GetTextureCount(ai_tex_type); ++i) {
		// Construct a texture object from the path in the material
		aiString local_tex_path;

		material->GetTexture(ai_tex_type, i, &local_tex_path);
		std::string path = modelDir + "/" + std::string(local_tex_path.C_Str());
		tex_list.emplace_back(scene_ref.lock()->GetTexture(path, custom_tex_type));
		
		// Note: if desired, you could also load more texture import options here, like
		//   wrapping, 2 sided, etc. General idea here:
		/*
		// Before the for loop...
		int type_counts[aiTextureType_UNKNOWN]{ 0 };
		// In the for loop...
		aiTextureType mapMode;
		aiReturn status = material->Get(AI_MATKEY_MAPPINGMODE_U(ai_tex_type,
		                                type_counts[ai_tex_type]), mapMode);
		if (status != AI_SUCCESS) {
			// Handle failure
		}
		// Make a mapping from aiTextureType to GLenum wrapType in TextureOptions
		// Increment the counter for this material type
		type_counts[ai_tex_type]++;
		*/
	}
}
