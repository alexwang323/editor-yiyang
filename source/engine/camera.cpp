#include "ginkgopch.h"
#include "camera.h"

void Camera::TranslateCamera(glm::vec3 translation) {
	position += translation;
	UpdateMatrix();
	UpdateProjectionMatrix();
}

void Camera::RotateCamera(float xRotation, float yRotation) {
	//std::cout << "Rotating camera" << std::endl;
	yaw += xRotation/* * sensitivity*/;
	pitch += yRotation/* * sensitivity*/;

	// Set boundary for pitch
	if (pitch > 89.0f) {
		pitch = 89.0f;
	}
	if (pitch < -89.0f) {
		pitch = -89.0f;
	}

	UpdateMatrix();
	UpdateProjectionMatrix();
}

void Camera::UpdateMatrix() {
	UpdateDirection();
	view = glm::lookAt(position, position + front, glm::vec3(0.0f, 1.0f, 0.0f));
}

void Camera::UpdateProjectionMatrix(float newFov, float newAspectRatio) {
	if (newFov > 0.0f) {
		fov = newFov;
	}
	if (newAspectRatio > 0.0f) {
		aspectRatio = newAspectRatio;
	}

	projection = glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
}

void Camera::UpdateDirection() {
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front = glm::normalize(front);

	right = glm::normalize(glm::cross(front, worldUp));
	up = glm::normalize(glm::cross(right, front));
}