#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "stb_image/stb_image.h"
#include "ShaderManager/ShaderManager.h"
#include "CameraManager/CameraManager.h"

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

// 创建贴图
unsigned int createTexture(const char* texturePath);
 
// 设置模型变化
void setModelTransform(ShaderManager& shader, glm::vec3 location, glm::vec3 scale);

int main()
{
	GLFWwindow* window = initWindow((int)screenWidth, (int)screenHeight);
	if (window == NULL)
		return -1;

	float vertices[] = {
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f, 0.0f, -1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  0.0f, 0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f, 0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f, 0.0f, -1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f, 0.0f, -1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f, 0.0f, -1.0f,

	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f, 0.0f,  1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, 0.0f,  1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.0f, 0.0f,  1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.0f, 0.0f,  1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  0.0f, 0.0f,  1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f, 0.0f,  1.0f,

	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  -1.0f, 0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  -1.0f, 0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  -1.0f, 0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  -1.0f, 0.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  -1.0f, 0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  -1.0f, 0.0f, 0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  1.0f,  0.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  1.0f,  0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  1.0f,  0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  1.0f,  0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  1.0f,  0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  1.0f,  0.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f, -1.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,  0.0f, -1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, -1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, -1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f, -1.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f, -1.0f, 0.0f,

	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,  0.0f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  1.0f, 0.0f
	};

	unsigned int indices[] = {
		0, 1, 3, // 第一个三角形
		1, 2, 3  // 第二个三角形
	};

	// VAO主要目的就是可以使用预设，VBO设置好之后就可以用VAO使用VBO的预设，我们在VBO里可以设置如何绑定vertex着色器的变量例如是需要绑定两个变量还是一个变量
	// 第一步先绑定渲染盒子VAO
	unsigned int VBO, CubeVAO , EBO;
	glGenVertexArrays(1, &CubeVAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(CubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);// 第二个参数是值vec中有多少个元素，而vec3是三个 这里其实再告诉gpu如何解释cpu传过去的数据
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(2);

	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); 这里不可以解绑EBO 因为 VAO只会记录GL_ELEMENT_ARRAY_BUFFER绑定。
	glBindVertexArray(0);
	// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); 因此如果在这里解绑是欧克的，因为VAO已经解绑了 不回记录这个buffer的解绑

	// 第二部 绑定灯光VAO
	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);// 第二个参数是值vec中有多少个元素，而vec3是三个 这里其实再告诉gpu如何解释cpu传过去的数据
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// shader
	ShaderManager shaderCube("./Engine/Shader/BasicLighting/VertexShader.glsl", "./Engine/Shader/BasicLighting/FragmentShader.glsl");
	ShaderManager shaderLight("./Engine/Shader/BasicLighting/VertexLightShader.glsl", "./Engine/Shader/BasicLighting/FragmentLightShader.glsl");

	// 加载贴图
	unsigned int texture1 = createTexture("./Materials/box.jpg");
	unsigned int texture2 = createTexture("./Materials/awesomeface.jpg");
	
	// 设置openGL状态
	glEnable(GL_DEPTH_TEST);

	while (!glfwWindowShouldClose(window))
	{
		// 每帧开始时计算时间
		calculateTime();

		// 处理输入
		proccessInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 先渲染盒子
		// 设置渲染所需要的贴图、顶点数据、矩阵
		glUseProgram(shaderCube.ID);
		glBindVertexArray(CubeVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);
		shaderCube.setInt("texture1", 0);
		shaderCube.setInt("texture2", 1);
		shaderCube.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
		shaderCube.setVec3("lightPos", glm::vec3(2.0f, 1.0f, 2.0f));
		shaderCube.setVec3("viewPos", cameraManager.getCameraPosition());

		// 设置物体旋转位置等
		setModelTransform(shaderCube,glm::vec3(0.0f),glm::vec3(1.0f));

		// 开始绘制
		glDrawArrays(GL_TRIANGLES, 0, 36);
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		// 在渲染灯光
		glUseProgram(shaderLight.ID);
		glBindVertexArray(shaderLight.ID);

		// 设置物体旋转位置等
		setModelTransform(shaderLight, glm::vec3(2.0f,1.0f,2.0f), glm::vec3(0.5f));

		// 开始绘制
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// 检查各种回调事件，鼠标键盘输入等
		glfwPollEvents();

		// swapbuffer
		glfwSwapBuffers(window);
	}

	glDeleteVertexArrays(1, &CubeVAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteProgram(shaderCube.ID);

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

unsigned int createTexture(const char* texturePath)
{
	// 0.翻转图片
	stbi_set_flip_vertically_on_load(true);
	
	// 1.从文件中加载贴图数据
	int width, height, nrChannels;
	unsigned char* data = stbi_load(texturePath, &width, &height, &nrChannels, 0);
	if (data == NULL)
	{
		std::cout << "Failed to load texture" << std::endl;
		stbi_image_free(data);
		return 0;
	}

	// 2.创建纹理
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	// 设置纹理环绕、过滤方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// 加载纹理
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	
	// 生成minimap
	glGenerateMipmap(texture);

	stbi_image_free(data);

	return texture;
}

void setModelTransform(ShaderManager& shader,glm::vec3 location,glm::vec3 scale)
{
	// 设置矩阵
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	//modelMatrix = glm::rotate(modelMatrix, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.0f, 1.0f, 0.0f)); 旋转
	modelMatrix = glm::scale(modelMatrix, scale);
	modelMatrix = glm::translate(modelMatrix, location);

	glm::mat4 viewMatrix = glm::mat4(1.0f);
	viewMatrix = cameraManager.getLookAtMatrix();

	glm::mat4 projectionMatrix = glm::mat4(1.0f);
	projectionMatrix = glm::perspective(glm::radians(45.f), screenWidth / screenHeight, 0.1f, 100.f);

	shader.setMatrix("modelMatrix", modelMatrix);
	shader.setMatrix("viewMatrix", viewMatrix);
	shader.setMatrix("projectionMatrix", projectionMatrix);
}
