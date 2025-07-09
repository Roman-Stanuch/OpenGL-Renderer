#pragma once

#include "glad/glad.h"
#include "glm/glm.hpp"

class Shader {
public:
	Shader(const char* vertexPath, const char* fragmentPath);

	void Use();

	void SetBool(const char* name, bool value) const;

	void SetInt(const char* name, int value) const;

	void SetFloat(const char* name, float value) const;

	void SetMat4(const char* name, const glm::mat4& value) const;

private:
	bool CheckShaderCompilation(GLuint shader, const char* identifier);

	bool CheckProgramLinking(GLuint program);

	GLuint programId;
};
