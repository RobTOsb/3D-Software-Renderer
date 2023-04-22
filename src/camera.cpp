#include "camera.h"

Camera::Camera()
{
	fov = glm::radians(90.0f);
	nearPlane = 0.1f;
	farPlane = 1000.0f;
	perspective = glm::perspective(fov, 16.0f / 9.0f, nearPlane, farPlane);
	view = glm::lookAt(position, target, up);
}

Camera::~Camera(){}

void Camera::UpdateView(glm::vec3 newPos)
{
	view = glm::lookAt(position, position + front, up);
}

void Camera::MoveForward(float delta)
{
	position += front * delta;
	UpdateView(position);
}

void Camera::MoveBackward(float delta)
{
	position -= front * delta;
	UpdateView(position);
}

void Camera::StrafeLeft(float delta)
{
	glm::vec3 right = glm::normalize(glm::cross(front, up));
	position -= right * delta;
	UpdateView(position);
}

void Camera::StrafeRight(float delta)
{
	glm::vec3 right = glm::normalize(glm::cross(front, up));
	position += right * delta;
	UpdateView(position);
}

//Handles the rotation of the front vector to allow for mouse control of camera
void Camera::RotateCamera(float deltaYaw, float deltaPitch) 
{
	yaw += deltaYaw;
	pitch += deltaPitch;

	// Limit the pitch angle to avoid gimbal lock
	if (pitch > 89.0f) {
		pitch = 89.0f;
	}
	if (pitch < -89.0f) {
		pitch = -89.0f;
	}

	// Calculate the new front vector based on the updated yaw and pitch angles
	glm::vec3 newFront;
	newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	newFront.y = sin(glm::radians(pitch));
	newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front = glm::normalize(newFront);

	// Update the view matrix
	view = glm::lookAt(position, position + front, up);
}


