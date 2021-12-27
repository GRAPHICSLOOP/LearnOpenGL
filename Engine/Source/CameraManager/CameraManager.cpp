#include "CameraManager.h"

CameraManager::CameraManager()
{
	cameraPos = glm::vec3(0.f);
	lookAtMat = glm::mat4(1.0f);
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

glm::mat4 CameraManager::getLookAtMatrix()
{
	if (!dirty)
		return lookAtMat;

	glm::vec3 cameraFront = glm::vec3(0.f, 0.f, -1.f);
	glm::vec3 cameraUp = glm::vec3(0.f, 1.f, 0.f);
	lookAtMat = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	return lookAtMat;
}
