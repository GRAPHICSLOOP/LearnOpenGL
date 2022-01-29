#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <ft2build.h>
#include <map>
#include FT_FREETYPE_H
#define FT_CONFIG_OPTION_ERROR_STRINGS

#include "stb_image/stb_image.h"
#include "ShaderManager/ShaderManager.h"
#include "CameraManager/CameraManager.h"
#include "Mesh/Model.h"
#include "Mesh/CubeLight.h"


#define screenWidth 800.f
#define screenHeight 800.f
#define CAMERASPEED 5.5f


GLenum glCheckError_(const char* file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case 0x0503:                error = "STACK_OVERFLOW"; break;
		case 0x0504:               error = "STACK_UNDERFLOW"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__) 

struct Character
{
	GLuint textureID; // 字形纹理ID
	glm::ivec2 size; // 字形大小
	glm::ivec2 bearing; // 从基准线到字形左部/顶部的偏移值
	GLuint Advance; // 原点距下一个字形原点的距离
};

// 摄像机系统
CameraManager cameraManager(glm::vec3(0.0f, 0.0f, 1.f), glm::vec3(0.0f, -90.0f, 0.0f));

float deltaTime = 0.0f; // 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间
float lastCursorX = 0.0f; // 上一帧的鼠标X轴位置
float lastCursorY = 0.0f; // 上一帧的鼠标Y轴位置
std::map<GLchar, Character> Characters;


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

// 获取模型矩阵
glm::mat4 getModelMatrix(glm::vec3 location, glm::vec3 scale, float rotation);

// 载入贴图
unsigned int loadTextureFromFile(const char* texturePath, GLenum clampType);

// 生成Cube
unsigned int createCube();

// 生成panel
unsigned int createPlane();

unsigned int createQuad();

void renderText(ShaderManager& shader, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color, GLuint VAO, GLuint VBO);

int main()
{
	GLFWwindow* window = initWindow((int)screenWidth, (int)screenHeight);
	if (window == NULL)
		return -1;

	glCheckError();

	// 文字初始化
	// ------------------------------------------------------------------
	FT_Error error_code;
	FT_Library ft;
	error_code = error_code = FT_Init_FreeType(&ft);
	if(error_code)
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

	FT_Face face;
	error_code = FT_New_Face(ft, "./Engine/fonts/arial.ttf", 0, &face);
	if (error_code)
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

	FT_Set_Pixel_Sizes(face, 0, 28);

	// 生成映射表
	// ------------------------------------------------------------------
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	for (FT_ULong c = 0; c < 128; c++)
	{
		// 加载字符的字形
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			
			continue;
		}

		// 生成纹理
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

		// 设置纹理选项
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// 储存纹理
		Character character =
		{
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};

		Characters.insert(std::pair<GLchar, Character>(c, character));
	}


	// 材质
	// ------------------------------------------------------------------
	ShaderManager textShader("./Engine/Shader/Font/vsFont.glsl", "./Engine/Shader/Font/fsFont.glsl");


	// 创建buffer
	GLuint VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


	// 设置透视矩阵
	glm::mat4 projection = glm::ortho(0.0f, screenWidth, 0.0f, screenHeight);
	glm::mat4 projectionMatrix = glm::mat4(1.0f);
	projectionMatrix = glm::perspective(glm::radians(90.f), screenWidth / screenHeight, 0.00001f, 10000.f);
	glm::mat4 modelMatrix = glm::mat4(1.f);
	textShader.use();
	textShader.setMatrix("projectionMatrix", projection);
	textShader.setMatrix("modelMatrix", modelMatrix); 

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	while (!glfwWindowShouldClose(window))
	{
		// 每帧开始时计算时间
		calculateTime();

		// 处理输入
		proccessInput(window);

		// 检查各种回调事件，鼠标键盘输入等
		glfwPollEvents();


		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//textShader.use();
		//modelMatrix = getModelMatrix(glm::vec3(0.f, 0.f, -1.f), glm::vec3(1.f / 400.f , 1.f / 400.f , 1.0f), (sin(lastFrame * 0.5) * 0.5 + 0.5) * 360.f);
		//textShader.setMatrix("modelMatrix", modelMatrix); 

		// 绘制
		// ------------------------------------------------------------------
		renderText(textShader, "aaaaaaaaaa", 0.0f, 0.0f, 1.0f, glm::vec3(1.0, 0.0f, 0.0f),VAO,VBO);
		//renderText(textShader, "(C) LearnOpenGL.com", 540.0f, 570.0f, 0.5f, glm::vec3(0.3, 0.7f, 0.9f),VAO, VBO);

		// swapbuffer
		glfwSwapBuffers(window);
	}

	// 清理
	FT_Done_Face(face);
	FT_Done_FreeType(ft);
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
	glfwWindowHint(GLFW_SAMPLES, 4);

	GLFWwindow* window = glfwCreateWindow(width, height, "LearnOpenGL", NULL, NULL);

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

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
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


	float cameraSpeed = CAMERASPEED * deltaTime;
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

void setModelTransform(ShaderManager& shader, glm::vec3 location, glm::vec3 scale, float rotation)
{
	// 设置矩阵
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	float rad = glm::radians(rotation);
	//modelMatrix = glm::rotate(modelMatrix, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.0f, 1.0f, 0.0f)); 旋转
	modelMatrix = glm::translate(modelMatrix, location) * glm::rotate(modelMatrix, rad, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::scale(modelMatrix, scale);

	glm::mat4 viewMatrix = glm::mat4(1.0f);
	viewMatrix = cameraManager.getLookAtMatrix();

	glm::mat4 projectionMatrix = glm::mat4(1.0f);
	projectionMatrix = glm::perspective(glm::radians(45.f), screenWidth / screenHeight, 0.1f, 100.f);

	shader.setMatrix("modelMatrix", modelMatrix);
	shader.setMatrix("viewMatrix", viewMatrix);
	shader.setMatrix("projectionMatrix", projectionMatrix);
}

glm::mat4 getModelMatrix(glm::vec3 location, glm::vec3 scale, float rotation)
{
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	float rad = glm::radians(rotation);
	modelMatrix = glm::translate(modelMatrix, location) * glm::rotate(modelMatrix, rad, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::scale(modelMatrix, scale);
	return modelMatrix;
}

unsigned int loadTextureFromFile(const char* texturePath, GLenum clampType)
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

unsigned int createCube()
{
	unsigned int cubeVAO, cubeVBO;
	float vertices[] = {
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
	};

	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);
	// fill buffer
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// link vertex attributes
	glBindVertexArray(cubeVAO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return cubeVAO;
}

unsigned int createPlane()
{
	unsigned int planeVAO, planeVBO;

	float planeVertices[] = {
		// positions            // normals         // texcoords
		 25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
		-25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
		-25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,

		 25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
		-25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,
		 25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,  25.0f, 25.0f
	};
	// plane VAO
	glGenVertexArrays(1, &planeVAO);
	glGenBuffers(1, &planeVBO);
	glBindVertexArray(planeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glBindVertexArray(0);

	return planeVAO;
}

unsigned int createQuad()
{
	unsigned int quadVAO, quadVBO;

	GLfloat quadVertices[] = {
		// Positions        // Texture Coords
		-1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
	};


	// Setup plane VAO
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));

	return quadVAO;
}

void renderText(ShaderManager& shader, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color,GLuint VAO, GLuint VBO)
{

	// 激活对应的渲染状态
	shader.use();
	shader.setVec3("textColor", color);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(VAO);

	// 遍历文本中的字符
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = Characters[*c];

		GLfloat xpos = x + ch.bearing.x * scale;
		GLfloat ypos = y - (ch.size.y - ch.bearing.y) * scale;
		GLfloat width = ch.size.x * scale;
		GLfloat height = ch.size.y * scale;

		// 对每个字符更新VBO
		GLfloat vertices[6][4]
		{
			{xpos,ypos + height,0.f,0.f},
			{xpos,ypos,0.f,1.f},
			{xpos + width,ypos,1.0f,1.0f},

			{xpos,ypos + height,0.f,0.f},
			{xpos + width,ypos,1.f,1.f},
			{xpos + width,ypos + height,1.f,0.f}
		};

		/*GLfloat vertices[6][4]
		{
			{0.f,10.f,0.f,0.f},
			{0.f,0.f,0.0f,1.f},
			{10.f,0.f,1.0f,1.0f},

			{0.f,10.f,0.f,0.f},
			{10.f,0.f,1.f,1.f},
			{10.f,10.f,1.f,1.f}
		};*/

		// 在四边形上绘制纹理
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

		glBindTexture(GL_TEXTURE_2D, ch.textureID);
		shader.setInt("text", 0);

		// 绘制四边形
		glDrawArrays(GL_TRIANGLES, 0, 6);
		x += (ch.Advance >> 6) * scale;
	}

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

}
