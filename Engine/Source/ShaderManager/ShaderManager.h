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

	// ����shader
	ShaderManager(const char* vertexPath,const char* fragmentPath);

	// ����shader
	void use();

	// uniform���ߺ���
	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
};

