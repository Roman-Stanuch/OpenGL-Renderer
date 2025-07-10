#include "Camera.h"
#include "glm/gtc/matrix_transform.hpp"

Camera::Camera(glm::vec3 startPosition, glm::vec3 startUp) {
	position = startPosition;
	front = glm::vec3(0.0f, 0.0f, -1.0f);
	worldUp = startUp;

	pitch = 0.0f;
	yaw = -90.0f;

	movementSpeed = 2.5f;
	sensitivity = 0.1f;
	zoom = 45.0f;

	UpdateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix() {
	return glm::lookAt(position, position + front, up);
}

void Camera::ProcessKeyboard(CameraMovement direction, float deltaTime) {
	float velocity = movementSpeed * deltaTime;

	if (direction == FORWARD) {
		position += front * velocity;
	}

	if (direction == BACKWARD) {
		position -= front * velocity;
	}

	if (direction == LEFT) {
		position -= right * velocity;
	}

	if (direction == RIGHT) {
		position += right * velocity;
	}
}

void Camera::ProcessMouseMovement(float xOffset, float yOffset, bool constrainPitch) {
	xOffset *= sensitivity;
	yOffset *= sensitivity;

	yaw += xOffset;
	pitch += yOffset;

	if (constrainPitch) {
		if (pitch > 89.0f) {
			pitch = 89.0f;
		}
		if (pitch < -89.0f) {
			pitch = -89.0f;
		}
	}

	UpdateCameraVectors();
}

void Camera::ProcessMouseScroll(float yOffset) {
	zoom -= (float)yOffset;

	if (zoom < 1.0f) {
		zoom = 1.0f;
	}

	if (zoom > 45.0f) {
		zoom = 45.0f;
	}
}

void Camera::UpdateCameraVectors() {
	glm::vec3 newFront;
	newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	newFront.y = sin(glm::radians(pitch));
	newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front = glm::normalize(newFront);
	right = glm::normalize(glm::cross(front, worldUp));
	up = glm::normalize(glm::cross(right, front));
}
