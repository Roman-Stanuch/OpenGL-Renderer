#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "stb/stb_image.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "Shader.h"
#include "Camera.h"

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
GLuint GetCubeVAO();
GLuint GetTexture(const char* source, bool flip = true);
void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void MouseMovementCallback(GLFWwindow* window, double xPos, double yPos);
void MouseScrollCallback(GLFWwindow* window, double xOffset, double yOffset);
void ProcessInput(GLFWwindow* window);

int main() {
	GLFWwindow* window = InitalizeWindow();
	if (window == nullptr) {
		return -1;
	}

	GLuint cubeVAO = GetCubeVAO();
	GLuint crateTexture = GetTexture("textures/container.jpg");
	GLuint awesomeFaceTexture = GetTexture("textures/awesomeface.png");
	Shader shaderProgram("shaders/vertex.vert", "shaders/fragment.frag");

	glfwSwapInterval(1);
	glEnable(GL_DEPTH_TEST);

	while (!glfwWindowShouldClose(window)) {
		// Calculate DeltaTime
		float currentTime = static_cast<float>(glfwGetTime());
		DeltaTime = currentTime - TimeLastFrame;
		TimeLastFrame = currentTime;

		// Input
		ProcessInput(window);

		// Rendering
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shaderProgram.Use();
		glm::mat4 model = glm::rotate(IDENTITY_4X4, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		shaderProgram.SetMat4("model", model);
		glm::mat4 view = MainCamera.GetViewMatrix();
		shaderProgram.SetMat4("view", view);
		glm::mat4 projection = glm::perspective(glm::radians(MainCamera.GetZoom()), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);
		shaderProgram.SetMat4("projection", projection);
		glBindVertexArray(cubeVAO);
		glBindTextureUnit(0, crateTexture);
		glBindTextureUnit(1, awesomeFaceTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// Swap Buffers and Poll Events
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
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

GLuint GetCubeVAO() {
	float vertices[] = {
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};

	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	return VAO;
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
