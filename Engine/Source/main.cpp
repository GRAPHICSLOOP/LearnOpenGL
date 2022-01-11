#include <glad/glad.h>
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

// 临时
void drawTwoBox(Model& mesh, ShaderManager& shaderModel, float Scale)
{
	// 渲染盒子模型
	shaderModel.setVec3("material.ambient", glm::vec3(0.2f));
	shaderModel.setFloat("material.shininess", 32.0f);
	setModelTransform(shaderModel, glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(Scale), 0.f);
	mesh.Draw(&shaderModel);

	// 渲染盒子模型
	shaderModel.setVec3("material.ambient", glm::vec3(0.2f));
	shaderModel.setFloat("material.shininess", 32.0f);
	setModelTransform(shaderModel, glm::vec3(0.0f, 0.0f, -2.0f), glm::vec3(Scale), 0.f);
	mesh.Draw(&shaderModel);
};

int main()
{
	GLFWwindow* window = initWindow((int)screenWidth, (int)screenHeight);
	if (window == NULL)
		return -1;

	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};

	// 模型
	Model box("./Model/box/Crate1.obj");
	CubeLight cubeLight("./Materials/box.jpg");

	// shader
	ShaderManager shaderLight("./Engine/Shader/MultiLight/VertexLightShader.glsl", "./Engine/Shader/MultiLight/FragmentLightShader.glsl");
	ShaderManager shaderModel("./Engine/Shader/Model/VertexShader.glsl", "./Engine/Shader/Model/FragmentShader.glsl");
	ShaderManager shaderOutLine("./Engine/Shader/Model/VertexShader.glsl", "./Engine/Shader/AdvancedOpenGL/FragOutLightShader.glsl");



	while (!glfwWindowShouldClose(window))
	{
		// 每帧开始时计算时间
		calculateTime();

		// 处理输入
		proccessInput(window);

		// 检查各种回调事件，鼠标键盘输入等
		glfwPollEvents();


		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		glUseProgram(shaderModel.ID);
		shaderModel.setVec3("viewPos", cameraManager.getCameraPosition());
		shaderModel.setVec3("pointLight.position", glm::vec3(2.0f, 1.0f, 2.0f));
		shaderModel.setVec3("pointLight.lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
		shaderModel.setFloat("pointLight.constant", 1.f);
		shaderModel.setFloat("pointLight.linear", 0.09f);
		shaderModel.setFloat("pointLight.quadratic", 0.032f);

		// 设置openGL状态
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_STENCIL_TEST);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

		glStencilMask(0x00);// 先关闭蒙版写入，防止更新地板

		// 渲染地板模型
		shaderModel.setVec3("material.ambient", glm::vec3(0.2f));
		shaderModel.setFloat("material.shininess", 32.0f);
		setModelTransform(shaderModel, glm::vec3(0.0f, -0.5f, 0.0f), glm::vec3(5.0f,0.02f,5.0f), 0.f);
		box.Draw(&shaderModel);

		/* 这个时候会将蒙版绘制成两个盒子 */

		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF);// 开启蒙版写入
		// 渲染盒子
		drawTwoBox(box, shaderModel, 0.5f);

		/* 增加描边，但是描边内部还是白色的，去掉内部的方式就是通过上面的glStencilOp
		* glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE)：如果其中的一个测试失败了，我们什么都不做，我们仅仅保留当前储存在模板缓冲中的值。如果模板测试和深度测试都通过了，那么我们希望将储存的模板值设置为参考值，参考值能够通过glStencilFunc来设置，我们之后会设置为1
		* 这样我们就得到了一个描边的蒙版，这样绘制的时候就只保留了描边内容，其余的都会被蒙版剔除掉
		*/

		glUseProgram(shaderOutLine.ID);
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x00);// 关闭蒙版写入
		glDisable(GL_DEPTH_TEST);
		// 渲染盒子
		drawTwoBox(box, shaderOutLine, 0.6f);
		glStencilMask(0xFF);// 开启蒙版写入
		glEnable(GL_DEPTH_TEST);



		// 渲染灯光
		glUseProgram(shaderLight.ID);
		setModelTransform(shaderLight, glm::vec3(2.0f,1.0f,2.0f), glm::vec3(0.5f),0.f);
		cubeLight.Draw(&shaderLight);

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
