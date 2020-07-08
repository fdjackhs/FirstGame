#pragma once
#include <map>
#include <vector>
#include <fstream>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include "Shader.h"
#include "Model.h"
#include "ObjectAttributes.h"
#include "ManuallyCreatedObject.h"

struct shader_path
{
	std::string vertex;
	std::string fragment;
};

struct model_bunch //path to model and type of shader for that model
{
	std::string path;
	std::string shader_type;
};

class ResourceManager
{
public:
	std::vector<std::string> listPathLevels;

	//first - type of shader, second - path to shader;
	std::map<std::string, shader_path> shaders_type_path; // load in constuctor
	//first - type of model, second - path to model;
	std::map<std::string, model_bunch> models_type_path; // load in constuctor

	std::vector<std::pair<std::string, Shader>> shaders;
	std::vector<std::pair<std::string, Model>> models;
	std::vector<ManuallyCreatedObject> m_manuallyCreaatedObjects;

	std::shared_ptr<Shader> m_stencilShader;

	//list of loaded pairs models & shaders
	//first - model index, second - shader index
	std::vector<std::pair<unsigned int, unsigned int>> modelIndex_shaderIndex;

	//manually created objects
	//first - manCrObj index, second - shader index
	std::vector<std::pair<unsigned int, unsigned int>> m_manCrObj_indexs;


	ResourceManager();

	//void init();
	void loadListPathLevels();
	void loadModelPairs(const char* path, std::map<std::string, model_bunch>& vector_pairs);
	void loadShaderPairs(const char* path, std::map<std::string, shader_path>& shader_pairs);
	void loadLevel(unsigned int number, std::vector<ObjectAttributes>& objectsAttrib);

	//return index - need for update
	uint32_t createObject(std::vector<GLfloat> vertices, const std::string& vertexPath, const std::string& fragmentPath);
	void updateVBO(uint32_t vbo, std::vector<GLfloat>& vertices);
};