#pragma once
#include "Mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "../stb_image/stb_image.h"

class Model
{
public:
	/* 函数 */

	Model(std::string path)
	{
		LoadModel(path);
	}

	void Draw(ShaderManager* shader);

private:
	/* 模型数据 */

	std::vector<Mesh> meshs; // 网格
	std::string directory; // 目录

	/* 函数 */

	void LoadModel(std::string path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, unsigned int typeName);
	unsigned int TextureFromFile(const char* texturePath);
};

