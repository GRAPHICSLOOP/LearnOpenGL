#pragma once

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class ShaderManager
{

public:
	// shader ID
	unsigned int ID;

	// 构造shader
	ShaderManager(const char* vertexPath,const char* fragmentPath);

	// 激活shader
	void use();

	// uniform工具函数
	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
	void setVec3(const std::string& name, glm::vec3 value) const;
	void setMatrix(const std::string& name,glm::mat4& mat);

private:
	// 加载shader从文件中
	unsigned int LoadShaderFromFile(const char* Path, GLenum type);
};

