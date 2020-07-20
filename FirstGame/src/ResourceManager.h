#pragma once
#include <map>
#include <vector>
#include <fstream>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Model.h"
#include "ObjectAttributes.h"
#include "ManuallyCreatedObject.h"

struct shader_path
{
	std::string vertex;
	std::string fragment;
};

struct rendering_parameters
{
	bool depth_test;
};

struct model_bunch //path to model and types of shaders for that model
{
	std::string path;
	// All shaders that may be needed to render this model (at least there should be BASIC and INSTANSING shaders)
	std::vector<std::string> shaders_types;
	rendering_parameters rend_params;
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
	std::vector<std::tuple<std::string, Model, rendering_parameters>> models;
	std::vector<ManuallyCreatedObject> m_manuallyCreaatedObjects;

	//std::shared_ptr<Shader> m_stencilShader;

	//list of loaded pairs models & shaders
	//first - model index, second - shader index
	std::vector<std::pair<unsigned int, std::vector<unsigned int>>> modelIndex_shadersIndices;

	//ready-made bundles of a list of models and shaders for them
	/*
		When a new object is created during the game, it receives a list of models 
		of which it consists (and shaders for specifically these models) of this map, 
		but during rendering this map is not used because the object (for example, a red unit) 
		already has a list of pair indices {model index, shader index} 
		which are stored in the std::vector modelIndex_shaderIndex 
	*/
	std::map<std::string, std::vector<unsigned int>> m_complete_models;

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

private:
	uint32_t getModelIndex(const std::string& type_of_model);
	std::vector<uint32_t> getShadersIndices(const std::string& type_of_shader);
	std::string readFile(const std::string& path);
};