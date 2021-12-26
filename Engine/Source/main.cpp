#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "ShaderManager/ShaderManager.h"
#include "stb_image/stb_image.h"

GLFWwindow* initWindow(int width, int height);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void proccessInput(GLFWwindow* window);

unsigned int createTexture(const char* texturePath);

const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";

const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\0";


int main()
{
	GLFWwindow* window = initWindow(800, 600);
	if (window == NULL)
		return -1;


	float vertices[] = {
		//     ---- 位置 ----       ---- 颜色 ----     - 纹理坐标 -
			 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // 右上
			 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // 右下
			-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // 左下
			-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // 左上
	};

	unsigned int indices[] = {
		0, 1, 3, // 第一个三角形
		1, 2, 3  // 第二个三角形
	};

	ShaderManager Shader("./Engine/Shader/HelloWorld/VertexShader.glsl", "./Engine/Shader/HelloWorld/GeometryShader.glsl");
	unsigned int shaderProgram = Shader.ID;

	unsigned int VBO, VAO , EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);// 第二个参数是值vec中有多少个元素，而vec3是三个 这里其实再告诉gpu如何解释cpu传过去的数据
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); 这里不可以解绑EBO 因为 VAO只会记录GL_ELEMENT_ARRAY_BUFFER绑定。
	glBindVertexArray(0);
	// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); 因此如果在这里解绑是欧克的，因为VAO已经解绑了 不回记录这个buffer的解绑


	// 加载贴图
	unsigned int texture = createTexture("./Materials/box.jpg");
	
	// 绑定调整窗口函数
	glfwSetFramebufferSizeCallback(window,framebuffer_size_callback);

	while (!glfwWindowShouldClose(window))
	{
		proccessInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(shaderProgram);

		glBindTexture(GL_TEXTURE_2D, texture);
		glBindVertexArray(VAO);

		//glDrawArrays(GL_TRIANGLES, 0, 6);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		// 检查各种回调事件，鼠标键盘输入等
		glfwPollEvents();

		// swapbuffer
		glfwSwapBuffers(window);
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteProgram(shaderProgram);

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

	return window;
}

void framebuffer_size_callback(GLFWwindow* window,int width,int height)
{
	std::cout << "change window size" << std::endl;
	glViewport(0, 0, width, height);
}

void proccessInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE))
	{
		glfwSetWindowShouldClose(window, true);
	}
}

unsigned int createTexture(const char* texturePath)
{
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