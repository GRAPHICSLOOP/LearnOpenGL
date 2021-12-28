#include "CameraManager.h"

CameraManager::CameraManager()
{
	cameraPos = glm::vec3(0.f);
	lookAtMat = glm::mat4(1.0f);
	cameraFront = glm::vec3(0.f, 0.f, -1.f);
	cameraUp = glm::vec3(0.f, 1.f, 0.f);
	dirty = true;
}

void CameraManager::setCameraPosition(glm::vec3 pos)
{
	cameraPos = pos;
	dirty = true;
	return;
}

void CameraManager::setCameraRotation(glm::vec3 rotation)
{
	// 限制旋转过头导致上下翻转
	if (glm::abs(rotation.x) > 89.0f)
		rotation.x = 89.0f * glm::sign(rotation.x);

	cameraRotation = rotation;
	dirty = true;
	return;
}

glm::vec3 CameraManager::getCameraPosition()
{
	return cameraPos;
}

glm::vec3& CameraManager::getCameraPositionRef()
{
	dirty = true;
	return cameraPos;
}

glm::mat4 CameraManager::getLookAtMatrix()
{
	if (!dirty)
		return lookAtMat;

	glm::vec3 front;
	float pitch = glm::radians(cameraRotation.x);
	float yaw = glm::radians(cameraRotation.y);
	front.x = cos(pitch) * cos(yaw);
	front.y = sin(pitch);
	front.z = cos(pitch) * sin(yaw);
	cameraFront = glm::normalize(front);

	lookAtMat = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	return lookAtMat;
}

glm::vec3 CameraManager::getCameraFrontDir()
{
	return cameraFront;
}

glm::vec3 CameraManager::getCaneraRightDir()
{
	return glm::normalize(glm::cross(cameraFront,cameraUp));
}

glm::vec3 CameraManager::getCameraRotation()
{
	return cameraRotation;
}

