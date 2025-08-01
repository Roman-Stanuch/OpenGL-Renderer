#include "Model.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"

#include <iostream>
#include <cstdint>

Model::Model(const std::string& path, bool flipTextures) {
	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cout << "ASSIMP ERROR: " << importer.GetErrorString() << "\n";
		return;
	}

	directory = path.substr(0, path.find_last_of('\\'));
	this->flipTextures = flipTextures;

	ProcessNode(scene->mRootNode, scene);
}

void Model::Draw(const Shader& shader) {
	for (Mesh& mesh : meshes) {
		mesh.Draw(shader);
	}
}

void Model::ProcessNode(aiNode* node, const aiScene* scene) {
	for (int i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(ProcessMesh(mesh, scene));
	}

	for (int i = 0; i < node->mNumChildren; i++) {
		ProcessNode(node->mChildren[i], scene);
	}
}

Mesh Model::ProcessMesh(aiMesh* mesh, const aiScene* scene) {
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	std::vector<Texture> textures;

	for (int i = 0; i < mesh->mNumVertices; i++) {
		Vertex vertex;

		vertex.Position.x = mesh->mVertices[i].x;
		vertex.Position.y = mesh->mVertices[i].y;
		vertex.Position.z = mesh->mVertices[i].z;

		vertex.Normal.x = mesh->mNormals[i].x;
		vertex.Normal.y = mesh->mNormals[i].y;
		vertex.Normal.z = mesh->mNormals[i].z;
		
		if (mesh->mTextureCoords[0]) {
			vertex.TexCoords.x = mesh->mTextureCoords[0][i].x;
			vertex.TexCoords.y = mesh->mTextureCoords[0][i].y;
		}
		else {
			vertex.TexCoords = glm::vec2(0.f, 0.f);
		}

		vertices.push_back(vertex);
	}

	for (int i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (int j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
	}
	
	if (mesh->mMaterialIndex >= 0) {
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		textures = LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
	}
	return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::LoadMaterialTextures(aiMaterial* material, aiTextureType type, const char* typeName) {
	std::vector<Texture> textures;
	for (int i = 0; i < material->GetTextureCount(type); i++) {
		aiString name;
		material->GetTexture(type, i, &name);
		
		// Check if texture was already loaded
		bool skip = false;
		for (const Texture& loadedTexture : loadedTextures) {
			if (std::strcmp(loadedTexture.GetFileName().data(), name.C_Str()) == 0) {
				textures.push_back(loadedTexture);
				skip = true;
				break;
			}
		}
		
		// Texture not loaded, load it
		if (!skip) {
			std::string texturePath = directory + '/' + name.C_Str();
			Texture texture(texturePath.c_str(), flipTextures);
			texture.SetFileName(name.C_Str());
			textures.push_back(texture);
			loadedTextures.push_back(texture);
		}
	}
	
	return textures;
}
