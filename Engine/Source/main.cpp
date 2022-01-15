﻿#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "stb_image/stb_image.h"
#include "ShaderManager/ShaderManager.h"
#include "CameraManager/CameraManager.h"
#include "Mesh/Model.h"
#include "Mesh/CubeLight.h"

#define screenWidth 800.f
#define screenHeight 600.f

// 摄像机系统
CameraManager cameraManager(glm::vec3(0.0f,0.0f,5.f),glm::vec3(0.0f,-90.0f,0.0f));

float deltaTime = 0.0f; // 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间
float lastCursorX = 0.0f; // 上一帧的鼠标X轴位置
float lastCursorY = 0.0f; // 上一帧的鼠标Y轴位置

// 初始化窗口
GLFWwindow* initWindow(int width, int height);

// 窗口尺寸改变回调函数
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

// 鼠标移动回调函数
void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);

// 计算事件插值
void calculateTime();

// 处理输入
void proccessInput(GLFWwindow* window);

// 设置模型变化
void setModelTransform(ShaderManager& shader, glm::vec3 location, glm::vec3 scale, float rotation);

// 载入贴图
unsigned int loadTextureFromFile(const char* texturePath, GLenum clampType);

int main()
{
	GLFWwindow* window = initWindow((int)screenWidth, (int)screenHeight);
	if (window == NULL)
		return -1;

	Model model("./Model/nanosuit/nanosuit.obj");
	
	// 加载材质
	// ------------------------------------------------------------------
	ShaderManager shader("./Engine/Shader/GeometryShader/VertexModelShader.glsl", "./Engine/Shader/GeometryShader/FragmentModelShader.glsl");
	//shader.linkShader("./Engine/Shader/GeometryShader/GeometryModelShader.glsl", GL_GEOMETRY_SHADER);
	ShaderManager normalShader("./Engine/Shader/GeometryShader/VertexModelShader.glsl", "./Engine/Shader/GeometryShader/FragmentShowNormalShader.glsl");
	normalShader.linkShader("./Engine/Shader/GeometryShader/GeometryShowNormalShader.glsl", GL_GEOMETRY_SHADER);

	glEnable(GL_DEPTH_TEST);
	while (!glfwWindowShouldClose(window))
	{
		// 每帧开始时计算时间
		calculateTime();

		// 处理输入
		proccessInput(window);

		// 检查各种回调事件，鼠标键盘输入等
		glfwPollEvents();

		// 绘制
		// ------------------------------------------------------------------
		glClearColor(0.2f, 0.3f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shader.use();
		setModelTransform(shader, glm::vec3(0.f), glm::vec3(0.2f), 0.f);
		//shader.setFloat("time", lastFrame);
		model.Draw(&shader);
		normalShader.use();
		setModelTransform(normalShader, glm::vec3(0.f), glm::vec3(0.2f), 0.f);
		model.Draw(&normalShader);

		// swapbuffer
		glfwSwapBuffers(window);
	}

	//glDeleteVertexArrays(1, &CubeVAO);
	//glDeleteBuffers(1, &VBO);
	//glDeleteBuffers(1, &EBO);
	//glDeleteProgram(shaderCube.ID);

	glfwTerminate();
	return 0;
}

GLFWwindow* initWindow(int width, int height)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);

	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return NULL;
	}

	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return NULL;
	}

	// 绑定调整窗口函数
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// 绑定鼠标移动
	glfwSetCursorPosCallback(window, cursorPosCallback);

	// 设置是否显示鼠标
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


	return window;
}

void framebuffer_size_callback(GLFWwindow* window,int width,int height)
{
	std::cout << "change window size" << std::endl;
	glViewport(0, 0, width, height);
}

void cursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
	float deltaCursorX = (float)xpos - lastCursorX;
	float deltaCursorY = lastCursorY - (float)ypos; 
	lastCursorX = (float)xpos;
	lastCursorY = (float)ypos;

	float sensitivity = 0.05f; // 旋转灵敏度
	deltaCursorY *= sensitivity;
	deltaCursorX *= sensitivity;

	// 设置旋转
	cameraManager.setCameraRotation(cameraManager.getCameraRotation() + glm::vec3(deltaCursorY, deltaCursorX, 0.0f));
}

void calculateTime()
{
	float currentFrame = (float)glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
}

void proccessInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE))
	{
		glfwSetWindowShouldClose(window, true);
	}


	float cameraSpeed = 1.5f * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		cameraManager.getCameraPositionRef() += cameraSpeed * cameraManager.getCameraFrontDir();
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		cameraManager.getCameraPositionRef() -= cameraSpeed * cameraManager.getCameraFrontDir();
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		cameraManager.getCameraPositionRef() += cameraSpeed * cameraManager.getCameraRightDir();
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		cameraManager.getCameraPositionRef() -= cameraSpeed * cameraManager.getCameraRightDir();
	}

	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{
		cameraManager.getCameraPositionRef() += cameraSpeed * cameraManager.getCameraUpDir();
	}

	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		cameraManager.getCameraPositionRef() -= cameraSpeed * cameraManager.getCameraUpDir();
	}
}

void setModelTransform(ShaderManager& shader,glm::vec3 location,glm::vec3 scale,float rotation)
{
	// 设置矩阵
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	//modelMatrix = glm::rotate(modelMatrix, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.0f, 1.0f, 0.0f)); 旋转
	modelMatrix = glm::translate(modelMatrix, location)*glm::rotate(modelMatrix, rotation, glm::vec3(1.0f, 0.3f, 0.5f)) *glm::scale(modelMatrix, scale);

	glm::mat4 viewMatrix = glm::mat4(1.0f);
	viewMatrix = cameraManager.getLookAtMatrix();

	glm::mat4 projectionMatrix = glm::mat4(1.0f);
	projectionMatrix = glm::perspective(glm::radians(45.f), screenWidth / screenHeight, 0.1f, 100.f);

	shader.setMatrix("modelMatrix", modelMatrix);
	shader.setMatrix("viewMatrix", viewMatrix);
	shader.setMatrix("projectionMatrix", projectionMatrix);
}

unsigned int loadTextureFromFile(const char* texturePath,GLenum clampType)
{
	// 0.翻转图片
	//stbi_set_flip_vertically_on_load(true);

	// 1.从文件中加载贴图数据
	int width, height, nrChannels;
	unsigned char* data = stbi_load(texturePath, &width, &height, &nrChannels, 0);
	if (data == NULL)
	{
		std::cout << "ERROR:Failed to load texture" << std::endl;
		stbi_image_free(data);
		return 0;
	}

	// 2.创建纹理
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	int format = nrChannels > 3 ? GL_RGBA : GL_RGB;

	// 加载纹理
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

	// 生成minimap
	glGenerateMipmap(texture);

	// 设置纹理环绕、过滤方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, clampType);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, clampType);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stbi_image_free(data);

	return texture;
}