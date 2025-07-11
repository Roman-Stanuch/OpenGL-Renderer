#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "stb/stb_image.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "Shader.h"
#include "Camera.h"
#include "Geometry.h"

#include <iostream>
#include <cstdint>

constexpr uint32_t SCREEN_WIDTH = 800;
constexpr uint32_t SCREEN_HEIGHT = 600;
constexpr glm::mat4 IDENTITY_4X4 = glm::mat4(1.0f);

Camera MainCamera(glm::vec3(0.0f, 0.0f, 3.0f));
bool FirstMouseMovement = true;
float LastMouseX;
float LastMouseY;
float DeltaTime = 0.0f;
float TimeLastFrame = 0.0f;

GLFWwindow* InitalizeWindow();
GLuint GetTexture(const char* source, bool flip = true);
void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void MouseMovementCallback(GLFWwindow* window, double xPos, double yPos);
void MouseScrollCallback(GLFWwindow* window, double xOffset, double yOffset);
void ProcessInput(GLFWwindow* window);
void PrintErrors();

int main() {
	GLFWwindow* window = InitalizeWindow();
	if (window == nullptr) {
		return -1;
	}
	
	// Models
	GLuint containerVAO = GetContainerVAO();
	glm::mat4 cubeModelMatrix = IDENTITY_4X4;

	GLuint lightCubeVAO = GetCubeVAO();
	glm::mat4 lightModelMatrix = glm::translate(IDENTITY_4X4, glm::vec3(1.2f, 1.0f, 2.0f));
	lightModelMatrix = glm::scale(lightModelMatrix, glm::vec3(0.2f));
	glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
	
	// Textures
	GLuint containerTexture = GetTexture("textures/container.jpg");
	GLuint awesomeFaceTexture = GetTexture("textures/awesomeface.png");

	// Shaders
	Shader containerShaderProgram("shaders/BasicTexture.vert", "shaders/BasicLighting.frag");
	Shader lightShaderProgram("shaders/BasicColor.vert", "shaders/BasicColor.frag");

	glfwSwapInterval(1);
	glEnable(GL_DEPTH_TEST);

	while (!glfwWindowShouldClose(window)) {
		//
		// Calculate DeltaTime
		//
		float currentTime = static_cast<float>(glfwGetTime());
		DeltaTime = currentTime - TimeLastFrame;
		TimeLastFrame = currentTime;
		
		//
		// Input
		//
		ProcessInput(window);
		
		//
		// Rendering
		//
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 view = MainCamera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(MainCamera.GetZoom()), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);

		// Draw the container
		containerShaderProgram.Use();
		containerShaderProgram.SetMat4("view", view);
		containerShaderProgram.SetMat4("projection", projection);
		containerShaderProgram.SetVec3("lightColor", lightColor);

		glBindVertexArray(containerVAO);
		containerShaderProgram.SetMat4("model", cubeModelMatrix);
		glBindTextureUnit(0, containerTexture);
		glBindTextureUnit(1, awesomeFaceTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	
		// Draw the light
		lightShaderProgram.Use();
		lightShaderProgram.SetMat4("view", view);
		lightShaderProgram.SetMat4("projection", projection);
		lightShaderProgram.SetVec3("lightColor", lightColor);

		glBindVertexArray(lightCubeVAO);
		lightShaderProgram.SetMat4("model", lightModelMatrix);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		
		//
		// Swap Buffers and Poll Events
		//
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	
	PrintErrors();
	glfwTerminate();
	return 0;
}

GLFWwindow* InitalizeWindow() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "OpenGL Renderer", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window\n";
		glfwTerminate();
		return nullptr;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
	glfwSetCursorPosCallback(window, MouseMovementCallback);
	glfwSetScrollCallback(window, MouseScrollCallback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD\n";
		return nullptr;
	}

	return window;
}

GLuint GetTexture(const char* source, bool flip) {
	GLuint texture = 0;
	int width;
	int height;
	int numberOfChannels;
	stbi_set_flip_vertically_on_load(flip);
	unsigned char* data = stbi_load(source, &width, &height, &numberOfChannels, 0);
	if (data) {
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		GLuint textureType = (numberOfChannels == 3) ? GL_RGB : GL_RGBA;
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, textureType, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "ERROR: Failed to load texture\n";
	}
	stbi_image_free(data);

	return texture;
}

void FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void MouseMovementCallback(GLFWwindow* window, double xPos, double yPos) {
	float xPosFloat = static_cast<float>(xPos);
	float yPosFloat = static_cast<float>(yPos);

	if (FirstMouseMovement) {
		LastMouseX = xPosFloat;
		LastMouseY = yPosFloat;
		FirstMouseMovement = false;
	}

	float xOffset = xPosFloat - LastMouseX;
	float yOffset = LastMouseY - yPosFloat;

	LastMouseX = xPosFloat;
	LastMouseY = yPosFloat;

	MainCamera.ProcessMouseMovement(xOffset, yOffset);
}

void MouseScrollCallback(GLFWwindow* window, double xOffset, double yOffset) {
	MainCamera.ProcessMouseScroll(static_cast<float>(yOffset));
}

void ProcessInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		MainCamera.ProcessKeyboard(FORWARD, DeltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		MainCamera.ProcessKeyboard(BACKWARD, DeltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		MainCamera.ProcessKeyboard(LEFT, DeltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		MainCamera.ProcessKeyboard(RIGHT, DeltaTime);
}

void PrintErrors() {
	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		std::cout << "ERROR: GL error code " << error << "\n";
	}
}
