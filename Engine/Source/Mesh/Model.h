#pragma once
#include "Mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "../stb_image/stb_image.h"

struct TextureRecorder {
	std::string fileName;
	unsigned int ID;
};

class Model
{
public:
	/* 函数 */

	Model(std::string path)
	{
		LoadModel(path);
	}

	void Draw(ShaderManager* shader);
	void DrawInstance(ShaderManager* shader,int instanceNumb);

	void setInstance(glm::mat4* model, unsigned int instanceNumb);
private:
	/* 模型数据 */

	std::vector<Mesh> meshs; // 网格
	std::string directory; // 目录
	std::vector<TextureRecorder> textureRecorders; // 已加载的贴图

	/* 函数 */

	void LoadModel(std::string path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, unsigned int typeName);
	unsigned int TextureFromFile(const char* texturePath);


};

