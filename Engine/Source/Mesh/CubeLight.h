#pragma once
#include <glad/glad.h>
#include <stdlib.h>
#include <iostream>
#include "../stb_image/stb_image.h"
#include "../ShaderManager/ShaderManager.h"

class CubeLight
{
private:
	unsigned int VAO, VBO;
	unsigned int texture;
public:

	CubeLight(const std::string& path);
	void Draw(ShaderManager* shader);
};

