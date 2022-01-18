#pragma once
// 相机使用右手坐标系
// +z是从屏幕指向你的 中指是x 拇指是y 食指是z

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class CameraManager
{
public:
	CameraManager();
	CameraManager(glm::vec3 pos, glm::vec3 rotation);

protected:
	glm::vec3 cameraPos;
	glm::vec3 cameraFront;
	glm::vec3 cameraUp;
	glm::vec3 cameraRotation;// 储存的是角度

	// 相机矩阵
private:
	glm::mat4 lookAtMat;
	bool dirty;

public:

	// 设置相机位置
	void setCameraPosition(glm::vec3 pos);
	// 设置相机旋转
	void setCameraRotation(glm::vec3 rotation);

	// 获取相机位置
	glm::vec3 getCameraPosition();
	glm::vec3& getCameraPositionRef();
	// 获取相机矩阵 -- 只有在调用该函数时候 相机才会更新场景位置，防止重复更新相机位置的时候多次计算矩阵 
	glm::mat4 getLookAtMatrix();
	// 获取相机向前的方向
	glm::vec3 getCameraFrontDir();
	// 获取相机向右的方向
	glm::vec3 getCameraRightDir();
	// 获取相机向上的方向
	glm::vec3 getCameraUpDir();
	// 获取相机旋转
	glm::vec3 getCameraRotation();

};

