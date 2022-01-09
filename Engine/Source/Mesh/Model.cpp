#include "Model.h"

void Model::Draw(ShaderManager* shader)
{
	unsigned int size = meshs.size();

	for (unsigned int i = 0; i < size; i++)
	{
		meshs[i].Draw(shader);
	}
}

void Model::LoadModel(std::string path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}

	// 保存目录
	directory = path.substr(0, path.find_last_of('/')) + "/";

	processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
	// 处理当前节点的所有mesh
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshs.push_back(processMesh(mesh, scene));
	}

	// 处理子节点
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		aiNode* childrenNode = node->mChildren[i];
		processNode(childrenNode, scene);
	}
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	Vertex vertex;
	// 处理顶点
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		vertex.position.x = mesh->mVertices[i].x;
		vertex.position.y = mesh->mVertices[i].y;
		vertex.position.z = mesh->mVertices[i].z;

		vertex.normal.x = mesh->mNormals[i].x;
		vertex.normal.y = mesh->mNormals[i].y;
		vertex.normal.z = mesh->mNormals[i].z;

		if (mesh->mTextureCoords[0])
		{
			vertex.textureCoords.x = mesh->mTextureCoords[0][i].x;
			vertex.textureCoords.y = mesh->mTextureCoords[0][i].y;
		}
		else
		{
			vertex.textureCoords = glm::vec2(0.0f);
		}

		vertices.push_back(vertex);
	}

	// 处理索引
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace* face = &mesh->mFaces[i];
		for (unsigned int j = 0; j < face->mNumIndices; j++)
		{
			indices.push_back(face->mIndices[j]);
		}
	}

	// 处理材质
	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, MESH_TEXTURE_DIFFUSE);
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end()); // 插入尾部

		std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, MESH_TEXTURE_SPECULAR);
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end()); // 插入尾部

	}

	return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, unsigned int typeName)
{
	std::vector<Texture> textures;
	Texture texture;

	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		aiString path;
		mat->GetTexture(type, i, &str);

		bool isSkip = false;
		for (unsigned int j = 0; j < textureRecorders.size(); j++)
		{
			if (textureRecorders[j].fileName == str.C_Str())
			{
				texture.ID = textureRecorders[j].ID;
				texture.type = typeName;
				textures.push_back(texture);
				isSkip = true;
				break;
			}
		}

		if (isSkip)
			continue;

		path.Set(directory.c_str());
		path.Append(str.C_Str());
		texture.ID = TextureFromFile(path.C_Str());
		texture.type = typeName;
		textures.push_back(texture);

		// 记录已加载的贴图
		TextureRecorder textureRecorder;
		textureRecorder.fileName = str.C_Str();
		textureRecorder.ID = texture.ID;
		textureRecorders.push_back(textureRecorder);
	}

	return textures;
}

unsigned int Model::TextureFromFile(const char* texturePath)
{
	// 0.翻转图片
	//stbi_set_flip_vertically_on_load(true);

	// 1.从文件中加载贴图数据
	int width, height, nrChannels;
	unsigned char* data = stbi_load(texturePath, &width, &height, &nrChannels, 0);
	if (data == NULL)
	{
		std::cout << "ERROR:Failed to load texture" << std::endl;
		stbi_image_free(data);
		return 0;
	}

	// 2.创建纹理
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	int format = nrChannels > 3 ? GL_RGBA : GL_RGB;

	// 加载纹理
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

	// 生成minimap
	glGenerateMipmap(texture);

	// 设置纹理环绕、过滤方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stbi_image_free(data);

	return texture;
}

