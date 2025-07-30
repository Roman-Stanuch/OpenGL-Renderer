#pragma once

#include "assimp/scene.h"

#include "Shader.h"
#include "Mesh.h"

#include <vector>
#include <string>

class Model {
public:
	Model(const std::string& path);

	void Draw(const Shader& shader);

private:
	std::vector<Mesh> meshes;

	std::string directory;

	void ProcessNode(aiNode* node, const aiScene* scene);

	Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);

	std::vector<Texture> LoadMaterialTextures(aiMaterial* material, aiTextureType type, const char* typeName);
};
