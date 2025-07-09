#include "Shader.h"
#include "glm/gtc/type_ptr.hpp"

#include <fstream>
#include <string>
#include <sstream>
#include <iostream>

Shader::Shader(const char* vertexPath, const char* fragmentPath) {
	std::string vertexCode;
	std::ifstream vertexShaderFile;
	vertexShaderFile.open(vertexPath);
	if (vertexShaderFile.good()) {
		std::stringstream vertexShaderStream;
		vertexShaderStream << vertexShaderFile.rdbuf();
		vertexShaderFile.close();
		vertexCode = vertexShaderStream.str();
	}
	else {
		std::cout << "ERROR: Could not open vertex shader file\n";
	}
	
	std::string fragmentCode;
	std::ifstream fragmentShaderFile;
	fragmentShaderFile.open(fragmentPath);
	if (fragmentShaderFile.good()) {
		std::stringstream fragmentShaderStream;
		fragmentShaderStream << fragmentShaderFile.rdbuf();
		fragmentShaderFile.close();
		fragmentCode = fragmentShaderStream.str();
	}
	else {
		std::cout << "ERROR: Could not open fragment shader file\n";
	}

	GLuint vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	const char* vertexShaderCode = vertexCode.c_str();
	glShaderSource(vertexShader, 1, &vertexShaderCode, NULL);
	glCompileShader(vertexShader);
	CheckShaderCompilation(vertexShader, "Vertex Shader");

	GLuint fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	const char* fragmentShaderCode = fragmentCode.c_str();
	glShaderSource(fragmentShader, 1, &fragmentShaderCode, NULL);
	glCompileShader(fragmentShader);
	CheckShaderCompilation(fragmentShader, "Fragment Shader");

	programId = glCreateProgram();
	glAttachShader(programId, vertexShader);
	glAttachShader(programId, fragmentShader);
	glLinkProgram(programId);
	CheckProgramLinking(programId);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void Shader::Use() {
	glUseProgram(programId);
}

void Shader::SetBool(const char* name, bool value) const {
	glUniform1i(glGetUniformLocation(programId, name), (int)value);
}

void Shader::SetInt(const char* name, int value) const {
	glUniform1i(glGetUniformLocation(programId, name), value);
}

void Shader::SetFloat(const char* name, float value) const {
	glUniform1f(glGetUniformLocation(programId, name), value);
}

void Shader::SetMat4(const char* name, const glm::mat4& value) const {
	glUniformMatrix4fv(glGetUniformLocation(programId, name), 1, GL_FALSE, &value[0][0]);
}

bool Shader::CheckShaderCompilation(GLuint shader, const char* identifier) {
	int success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		std::cout << "ERROR: " << identifier << " compilation failed\n" << infoLog << "\n";
		return false;
	};

	return true;
}

bool Shader::CheckProgramLinking(GLuint program) {
	int success;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetProgramInfoLog(program, 512, NULL, infoLog);
		std::cout << "ERROR: Shader program linking failed\n" << infoLog << "\n";
		return false;
	}

	return true;
}
