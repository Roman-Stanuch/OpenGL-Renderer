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
#include "Model.h"

#include <iostream>
#include <cstdint>

constexpr uint32_t SCREEN_WIDTH = 1920;
constexpr uint32_t SCREEN_HEIGHT = 1080;
constexpr uint32_t DIALOG_WIDTH = static_cast<uint32_t>(SCREEN_WIDTH * 0.8);
constexpr uint32_t DIALOG_HEIGHT = static_cast<uint32_t>(SCREEN_HEIGHT * 0.8);
constexpr glm::mat4 IDENTITY_4X4 = glm::mat4(1.0f);

Camera MainCamera(glm::vec3(0.0f, 0.0f, 3.0f));
bool FirstMouseMovement = true;
float LastMouseX;
float LastMouseY;
float DeltaTime = 0.0f;
float TimeLastFrame = 0.0f;

Model* LoadedModel = nullptr;
bool FlipModelTextures = false;

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
	
	// Model transforms
	glm::mat4 modelMatrix = IDENTITY_4X4;
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.f, 0.f, 0.f));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(1.f, 1.f, 1.f));

	// Shaders
	Shader modelShaderProgram("shaders/BasicTexture.vert", "shaders/BasicTexture.frag");

	glfwSwapInterval(1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

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
		modelShaderProgram.Use();
		modelShaderProgram.SetMat4("view", view);
		modelShaderProgram.SetMat4("projection", projection);
		modelShaderProgram.SetMat4("model", modelMatrix);
		if (LoadedModel != nullptr) {
			LoadedModel->Draw(modelShaderProgram);
		}

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

	// Window
	ImGui::SetNextWindowSize(ImVec2(SCREEN_WIDTH, 0.f));
	ImGui::SetNextWindowPos(ImVec2(0.f, 0.f));
	ImGui::Begin("Main Window", nullptr, ImGuiWindowFlags_NoTitleBar);

	// Header Buttons 
	if (ImGui::Button("Open Model")) {
		IGFD::FileDialogConfig config;
		config.path = ".";
		ImGui::SetNextWindowSize(ImVec2(DIALOG_WIDTH, DIALOG_HEIGHT));
		ImGuiFileDialog::Instance()->OpenDialog("OpenModelDialog", "Open Model", ".obj", config);
	}
	
	ImGui::Checkbox("Flip Textures", &FlipModelTextures);
	
	// File Dialog
	if (ImGuiFileDialog::Instance()->Display("OpenModelDialog")) {
		if (ImGuiFileDialog::Instance()->IsOk()) {
			std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
			if (LoadedModel != nullptr) {
				delete LoadedModel;
			}
			LoadedModel = new Model(filePathName, FlipModelTextures);
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
