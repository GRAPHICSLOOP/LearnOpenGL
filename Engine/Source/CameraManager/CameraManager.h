#pragma once
// 相机使用左手坐标系

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class CameraManager
{
public:
	CameraManager();

protected:
	glm::vec3 cameraPos;
	glm::vec3 cameraFront;
	glm::vec3 cameraUp;

	// 相机矩阵
private:
	glm::mat4 lookAtMat;
	bool dirty;

public:

	// 设置相机位置
	void setCameraPosition(glm::vec3 pos);
	// 获取相机位置
	glm::vec3 getCameraPosition();
	glm::vec3& getCameraPositionRef();
	// 获取相机矩阵
	glm::mat4 getLookAtMatrix();
	// 获取相机向前的方向
	glm::vec3 getCameraFrontDir();
	// 获取相机向右的方向
	glm::vec3 getCaneraRightDir();
};

