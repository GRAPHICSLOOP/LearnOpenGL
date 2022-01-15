#include "ShaderManager.h"

ShaderManager::ShaderManager(const char* vertexPath, const char* fragmentPath)
{
	// 编译shader
	unsigned int vertexShader, fragmentShader;
	int success;
	char infoLog[512];
	vertexShader = loadShaderFromFile(vertexPath, GL_VERTEX_SHADER);
	fragmentShader = loadShaderFromFile(fragmentPath, GL_FRAGMENT_SHADER);

	// shader 程序
	ID = glCreateProgram();
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader);
	glLinkProgram(ID);

	// 打印错误
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	// 删除shader，它们以及链接到程序中，因此不在需要
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void ShaderManager::use()
{
	glUseProgram(ID);
}

void ShaderManager::setBool(const std::string& name, bool value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void ShaderManager::setInt(const std::string& name, int value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void ShaderManager::setFloat(const std::string& name, float value) const
{
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void ShaderManager::setVec3(const std::string& name, glm::vec3 value) const
{
	glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1,glm::value_ptr(value));
}

void ShaderManager::setVec2(const std::string& name, glm::vec2 value) const
{
	glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value));
}

void ShaderManager::setMatrix(const std::string& name, glm::mat4& mat)
{
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

void ShaderManager::linkShader(const char* shaderPath, GLenum type)
{
	unsigned int shader = loadShaderFromFile(shaderPath, type);
	glAttachShader(ID, shader);
	glLinkProgram(ID);
	int success;
	char infoLog[512];
	// 打印错误
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(shader);
}

unsigned int ShaderManager::loadShaderFromFile(const char* Path, GLenum type)
{
	// 1.从文件路径中获取 vertex/fragment shader
	std::string shaderCode;
	std::ifstream shaderFile;

	// 抛出异常
	shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		// 打开文件
		shaderFile.open(Path);

		std::stringstream shaderStream;
		// 读取文件内容到数据流中
		shaderStream << shaderFile.rdbuf();

		// 转换为string
		shaderCode = shaderStream.str();

	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}

	const char* sCode = shaderCode.c_str();

	// 2.编译shader
	unsigned int shader;
	int success;
	char infoLog[512];

	// shader
	shader = glCreateShader(type);
	glShaderSource(shader, 1, &sCode, NULL);
	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		std::string shaderName = "VERTEX";
		if (type != GL_VERTEX_SHADER)
			shaderName = "FRAGMENT";

		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::"<< shaderName << "::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	return shader;
}

