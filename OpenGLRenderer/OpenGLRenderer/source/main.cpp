#include "IMGUI/imgui.h"
#include "IMGUI/imgui_impl_glfw.h"
#include "IMGUI/imgui_impl_opengl3.h"
#include "ImGuiFileDialog/ImGuiFileDialog.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "Shader.h"
#include "Camera.h"
#include "Geometry.h"
#include "Texture.h"

#include <iostream>
#include <cstdint>

constexpr uint32_t SCREEN_WIDTH = 800;
constexpr uint32_t SCREEN_HEIGHT = 600;
constexpr uint32_t DIALOG_WIDTH = static_cast<uint32_t>(SCREEN_WIDTH * 0.8);
constexpr uint32_t DIALOG_HEIGHT = static_cast<uint32_t>(SCREEN_HEIGHT * 0.8);
constexpr glm::mat4 IDENTITY_4X4 = glm::mat4(1.0f);

Camera MainCamera(glm::vec3(0.0f, 0.0f, 3.0f));
bool FirstMouseMovement = true;
float LastMouseX;
float LastMouseY;
float DeltaTime = 0.0f;
float TimeLastFrame = 0.0f;

GLFWwindow* InitalizeWindow();
void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void MouseMovementCallback(GLFWwindow* window, double xPos, double yPos);
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void MouseScrollCallback(GLFWwindow* window, double xOffset, double yOffset);
void UpdateDeltaTime();
void DrawGui();
void ShutdownRenderer();
void ProcessInput(GLFWwindow* window);
void PrintErrors();

int main() {
	GLFWwindow* window = InitalizeWindow();
	if (window == nullptr) {
		return -1;
	}
	
	// Models
	GLuint containerVAO = GetContainerVAO();
	glm::mat4 containerModelMatrix = IDENTITY_4X4;
	glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
	
	// Textures
	Texture containerTexture("textures/container.jpg");
	Texture awesomeFaceTexture("textures/awesomeface.png");

	// Shaders
	Shader containerShaderProgram("shaders/BasicTexture.vert", "shaders/BasicLighting.frag");
	Shader lightShaderProgram("shaders/BasicColor.vert", "shaders/BasicColor.frag");

	glfwSwapInterval(1);
	glEnable(GL_DEPTH_TEST);

	while (!glfwWindowShouldClose(window)) {
		//
		// Framestart 
		//
		UpdateDeltaTime();
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
		containerShaderProgram.SetMat4("model", containerModelMatrix);
		containerTexture.Activate(0);
		awesomeFaceTexture.Activate(1);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// Draw the GUI
		DrawGui();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		
		//
		// Swap Buffers and Poll Events
		//
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	ShutdownRenderer();
	
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
	glfwSetMouseButtonCallback(window, MouseButtonCallback);
	glfwSetScrollCallback(window, MouseScrollCallback);


	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD\n";
		return nullptr;
	}

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	return window;
}

void FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void MouseMovementCallback(GLFWwindow* window, double xPos, double yPos) {
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) != GLFW_PRESS) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		FirstMouseMovement = true;
		return;
	}

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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

	MainCamera.ProcessMouseMovement(xOffset, yOffset, true);
}

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
}

void MouseScrollCallback(GLFWwindow* window, double xOffset, double yOffset) {
	MainCamera.ProcessMouseScroll(static_cast<float>(yOffset));
}

void UpdateDeltaTime() {
	float currentTime = static_cast<float>(glfwGetTime());
	DeltaTime = currentTime - TimeLastFrame;
	TimeLastFrame = currentTime;
}

void DrawGui() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::SetNextWindowSize(ImVec2(SCREEN_WIDTH, 0.f));
	ImGui::SetNextWindowPos(ImVec2(0.f, 0.f));
	ImGui::Begin("Main Window", nullptr, ImGuiWindowFlags_NoTitleBar);

	if (ImGui::Button("Open Model")) {
		IGFD::FileDialogConfig config;
		config.path = ".";
		ImGui::SetNextWindowSize(ImVec2(DIALOG_WIDTH, DIALOG_HEIGHT));
		ImGuiFileDialog::Instance()->OpenDialog("OpenModelDialog", "Open Model", ".obj", config);
	}
	if (ImGuiFileDialog::Instance()->Display("OpenModelDialog")) {
		if (ImGuiFileDialog::Instance()->IsOk()) {
			std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
			std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
			std::cout << "Object Selected: " << ImGuiFileDialog::Instance()->GetCurrentFileName() << " at " << filePathName << "\n";
			ImGuiFileDialog::Instance()->Close();
		}

		ImGuiFileDialog::Instance()->Close();
	}

	ImGui::End();
}

void ShutdownRenderer() {
	PrintErrors();
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
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
