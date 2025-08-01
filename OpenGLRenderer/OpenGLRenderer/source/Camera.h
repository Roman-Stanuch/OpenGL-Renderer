#pragma once

#include "glm/glm.hpp"

enum CameraMovement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

class Camera {
public:
	Camera(glm::vec3 startPosition = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 startUp = glm::vec3(0.0f, 1.0f, 0.0f));

	glm::mat4 GetViewMatrix();

	void ProcessKeyboard(CameraMovement direction, float deltaTime);

	void ProcessMouseMovement(float xOffset, float yOffset, bool constrainPitch = true);

	void ProcessMouseScroll(float yOffset);

	float GetZoom() const { return zoom; }

	void SetSpeed(float speed) { movementSpeed = speed; }

private:
	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 worldUp;
	
	float pitch;
	float yaw;
	float movementSpeed;
	float sensitivity;
	float zoom;

	void UpdateCameraVectors();
};
