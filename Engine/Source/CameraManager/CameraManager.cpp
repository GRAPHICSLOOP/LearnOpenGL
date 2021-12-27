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
