#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class CameraManager
{
public:
	CameraManager();

private:
	glm::vec3 cameraPos;
	glm::mat4 lookAtMat;
	bool dirty;

public:
	void setCameraPosition(glm::vec3 pos);
	glm::vec3 getCameraPosition();
	glm::mat4 getLookAtMatrix();
};

