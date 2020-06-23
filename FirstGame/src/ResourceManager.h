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

struct shader_path
{
	std::string vertex;
	std::string fragment;
};

class ResourceManager
{
public:
	std::vector<std::string> listPathLevels;

	//first - type of shader, second - path to shader;
	std::map<std::string, shader_path> shaders_type_path; // load in constuctor
	//first - type of model, second - path to model;
	std::map<std::string, std::string> models_type_path; // load in constuctor

	std::vector<std::pair<std::string, Shader>> shaders;
	std::vector<std::pair<std::string, Model>> models;

	//list of loaded pairs models & shaders
	//first - model index, second - shader index
	std::vector<std::pair<unsigned int, unsigned int>> modelIndex_shaderIndex;

	ResourceManager();

	//void init();
	void loadListPathLevels();
	void loadPairs(const char* path, std::map<std::string, std::string>& vector_pairs);
	void loadShaderPairs(const char* path, std::map<std::string, shader_path>& shader_pairs);
	void loadLevel(unsigned int number, std::vector<ObjectAttributes>& objectsAttrib);
};