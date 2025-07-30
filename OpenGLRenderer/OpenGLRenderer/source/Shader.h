#pragma once

#include "glm/glm.hpp"

#include <cstdint>

class Shader {
public:
	Shader(const char* vertexPath, const char* fragmentPath);

	void Use();

	void SetBool(const char* name, bool value) const;

	void SetInt(const char* name, int value) const;

	void SetFloat(const char* name, float value) const;

	void SetVec3(const char* name, const glm::vec3& value) const;

	void SetMat4(const char* name, const glm::mat4& value) const;

private:
	bool CheckShaderCompilation(uint32_t shader, const char* identifier);

	bool CheckProgramLinking(uint32_t program);

	uint32_t programId;
};
