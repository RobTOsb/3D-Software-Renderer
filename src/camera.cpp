#include "camera.h"

Camera::Camera()
{
	fov = glm::radians(90.0f);
	nearPlane = 0.1f;
	farPlane = 1000.0f;
	perspective = glm::perspective(fov, (float)Display::SCREENWIDTH / Display::SCREENHEIGHT, nearPlane, farPlane);
	view = glm::lookAt(position, target, up);

	calculateFrustumPlanes();
}

Camera::~Camera(){}

// Calculate the frustum planes from the view matrix.
void Camera::calculateFrustumPlanes()
{
	glm::mat4x4 viewMatrix = getView();

	frustumPlanes.resize(6);

	// Calculate the near plane.
	glm::vec4 nearPlane = glm::vec4(viewMatrix[0][2], viewMatrix[1][2], viewMatrix[2][2], viewMatrix[3][2]);
	nearPlane /= glm::length(glm::vec3(nearPlane));

	// Calculate the far plane.
	glm::vec4 farPlane = glm::vec4(-viewMatrix[0][2], -viewMatrix[1][2], -viewMatrix[2][2], -viewMatrix[3][2]);
	farPlane /= glm::length(glm::vec3(farPlane));

	// Calculate the left plane.
	glm::vec4 leftPlane = glm::vec4(viewMatrix[0][3] + viewMatrix[0][0], viewMatrix[1][3] + viewMatrix[1][0], viewMatrix[2][3] + viewMatrix[2][0], viewMatrix[3][3] + viewMatrix[3][0]);
	leftPlane /= glm::length(glm::vec3(leftPlane));

	// Calculate the right plane.
	glm::vec4 rightPlane = glm::vec4(viewMatrix[0][3] - viewMatrix[0][0], viewMatrix[1][3] - viewMatrix[1][0], viewMatrix[2][3] - viewMatrix[2][0], viewMatrix[3][3] - viewMatrix[3][0]);
	rightPlane /= glm::length(glm::vec3(rightPlane));

	// Calculate the top plane.
	glm::vec4 topPlane = glm::vec4(viewMatrix[0][3] - viewMatrix[0][1], viewMatrix[1][3] - viewMatrix[1][1], viewMatrix[2][3] - viewMatrix[2][1], viewMatrix[3][3] - viewMatrix[3][1]);
	topPlane /= glm::length(glm::vec3(topPlane));

	// Calculate the bottom plane.
	glm::vec4 bottomPlane = glm::vec4(viewMatrix[0][3] + viewMatrix[0][1], viewMatrix[1][3] + viewMatrix[1][1], viewMatrix[2][3] + viewMatrix[2][1], viewMatrix[3][3] + viewMatrix[3][1]);
	bottomPlane /= glm::length(glm::vec3(bottomPlane));

	// Set the frustum planes.
	frustumPlanes[0] = nearPlane;
	frustumPlanes[1] = farPlane;
	frustumPlanes[2] = leftPlane;
	frustumPlanes[3] = rightPlane;
	frustumPlanes[4] = topPlane;
	frustumPlanes[5] = bottomPlane;
}


void Camera::UpdateView(glm::vec3 newPos)
{
	view = glm::lookAt(position, position + front, up);
	calculateFrustumPlanes();
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


