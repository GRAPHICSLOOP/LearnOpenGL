#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include "../ShaderManager/ShaderManager.h"

#define MESH_TEXTURE_DIFFUSE 0x0001
#define MESH_TEXTURE_SPECULAR 0x0002

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 textureCoords;
};

struct Texture {
	unsigned int ID;
	int type;
};

class Mesh
{
public:
	/* 网格数据 */

	std::vector<Vertex> vertices; // 顶点
	std::vector<Texture> textures;// 贴图
	std::vector<unsigned int> indices;// 索引

	/* 函数 */

	// 构造
	Mesh(std::vector<Vertex> v,std::vector<unsigned int> i, std::vector<Texture> t);
	
	// 绘制
	void Draw(ShaderManager* shader);
	void DrawInstance(ShaderManager* shader,int instanceNumb);

	unsigned int getVAO()
	{
		return VAO;
	}
private:

	/* 渲染数据 */
	unsigned int VAO, VBO, EBO;

	/* 函数 */
	void SetUpMesh();
};

