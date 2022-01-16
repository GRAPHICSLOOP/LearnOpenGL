#include "Mesh.h"

Mesh::Mesh(std::vector<Vertex> v, std::vector<unsigned int> i, std::vector<Texture> t)
{
	vertices = v;
	indices = i;
	textures = t;

	SetUpMesh();
}

void Mesh::Draw(ShaderManager* shader)
{
	glBindVertexArray(VAO);

	unsigned int size = textures.size(); 
	std::string name = ""; // 贴图名称
	std::string numb = ""; // 贴图位置
	for (unsigned int i = 0; i < size; i++)
	{
		/* 激活贴图 */
		glActiveTexture(GL_TEXTURE0 + i);

		/* 绑定 */
		glBindTexture(GL_TEXTURE_2D, textures[i].ID);

		/* 设置 */ 

		if (textures[i].type == MESH_TEXTURE_DIFFUSE)
		{
			name = "diffuse";
		}
		else if (textures[i].type == MESH_TEXTURE_SPECULAR)
		{
			name = "specular";
		}
		numb = std::to_string(i);

		shader->setInt("material." + name + numb, i);
	}

	/* 绘制网格 */

	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Mesh::DrawInstance(ShaderManager* shader, int instanceNumb)
{
	glBindVertexArray(VAO);

	unsigned int size = textures.size();
	std::string name = ""; // 贴图名称
	std::string numb = ""; // 贴图位置
	for (unsigned int i = 0; i < size; i++)
	{
		/* 激活贴图 */
		glActiveTexture(GL_TEXTURE0 + i);

		/* 绑定 */
		glBindTexture(GL_TEXTURE_2D, textures[i].ID);

		/* 设置 */

		if (textures[i].type == MESH_TEXTURE_DIFFUSE)
		{
			name = "diffuse";
		}
		else if (textures[i].type == MESH_TEXTURE_SPECULAR)
		{
			name = "specular";
		}
		numb = std::to_string(i);

		shader->setInt("material." + name + numb, i);
	}

	/* 绘制网格 */
	glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT,0,instanceNumb);
	glBindVertexArray(0);
}

void Mesh::SetUpMesh()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	/* 处理顶点相关 */

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW); // 这里需要特别注意大小是值类型大小乘以个数

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);// 第二个参数是值vec中有多少个元素，而vec3是三个 这里其实再告诉gpu如何解释cpu传过去的数据
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, textureCoords));
	glEnableVertexAttribArray(2);

	/* 处理顶点索引相关 */

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	/* 结束绑定 */

	glBindVertexArray(0);
}