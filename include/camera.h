//Class for camera to view from

#ifndef CAMERA_H
#define CAMERA_H

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "display.h"
#include <vector>

class Camera
{
public:
	Camera();
	~Camera();

	glm::vec3 position = { 0.0f, 0.0f, 3.0f };
	glm::vec3 up = { 0.0f, 1.0f, 0.0f };
	glm::vec3 front = { 0.f, 0.f, -1.f };
	glm::vec3 target = { 0.f, 0.f, 0.f };
	glm::mat4 getPerspective() { return perspective; }
	glm::mat4 getView() { return view; }
	std::vector<glm::vec4> getPlanes() { return frustumPlanes; }

	void calculateFrustumPlanes();

	void UpdateView(glm::vec3 newPos);
	void MoveForward(float delta);
	void MoveBackward(float delta);
	void StrafeLeft(float delta);
	void StrafeRight(float delta);

	void RotateCamera(float deltaYaw, float deltaPitch);

private:

	glm::mat4x4 perspective;
	glm::mat4x4 view;
	float fov;
	float nearPlane;
	float farPlane;

	std::vector<glm::vec4> frustumPlanes;

	float yaw = -90.0f;
	float pitch = 0.0f;


};

#endif
