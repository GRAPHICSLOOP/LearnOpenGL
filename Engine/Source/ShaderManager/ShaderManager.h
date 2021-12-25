#pragma once

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

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
};

