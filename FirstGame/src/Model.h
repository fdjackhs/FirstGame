#pragma once
#include <vector>
#include <string>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <stb_image.h>

#include "Mesh.h"
#include "Shader.h"

unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma = false);

class Model
{
public:
	std::vector<Texture> textures_loaded;
	std::vector<Mesh> meshes;
	std::string directory;
	bool gammaCorrection;

	bool verticesUpdated;

	Model(const char *path, bool gamma = false) : gammaCorrection(gamma)
	{
		verticesUpdated = false;
		loadModel(path);
	}
	void Draw(Shader shader);
private:

	void loadModel(std::string path);
	void processNode(aiNode *node, const aiScene *scene, int depth);
	Mesh processMesh(aiMesh *mesh, const aiScene *scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
};