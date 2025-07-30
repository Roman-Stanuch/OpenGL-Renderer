#pragma once

#include "glm/glm.hpp"

#include "Texture.h"
#include "Shader.h"

#include <vector>
#include <cstdint>

struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
};

class Mesh {
public:
	Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices, std::vector<Texture> textures);

	void Draw(const Shader& shader);
	
private:
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	std::vector<Texture> textures;

	uint32_t VAO = 0;
	uint32_t VBO = 0;
	uint32_t EBO = 0;

	void SetupMesh();
};
