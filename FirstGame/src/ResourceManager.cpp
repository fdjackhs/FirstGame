#include "ResourceManager.h"

ResourceManager::ResourceManager()
{
	ResourceManager::loadListPathLevels();
	loadShaderPairs("../FirstGame/Resources/ResourceManager/shaders_type_path.txt", m_shaders_type_path);
	loadModelPairs("../FirstGame/Resources/ResourceManager/models_type_path.txt", m_models_type_path);

	while (false);

}

void ResourceManager::loadListPathLevels()
{
	std::ifstream levelPathsFile;

	levelPathsFile.open("../FirstGame/Resources/ResourceManager/list_path_levels.txt");

	if (levelPathsFile)
	{
		std::string path;
		while (std::getline(levelPathsFile, path))
			m_listPathLevels.push_back(path);

		levelPathsFile.close();
	}
	else
	{
		std::cout << "ERROR::ResourceManager: list_path_levels.txt file not sucessfully read" << std::endl;
	}
}

void ResourceManager::loadModelPairs(const char* path, std::map<std::string, model_bunch>& vector_pairs)
{
	std::ifstream pair_File;
	pair_File.open(path);

	if (pair_File)
	{
		std::string pairs;
		std::stringstream stream_pairs;
		stream_pairs << pair_File.rdbuf();
		pairs = stream_pairs.str();

		rapidjson::Document d;
		d.Parse(pairs.c_str());

		for (unsigned int i = 0; i < d.Size(); i++)
		{
			rapidjson::Value& shaders_types = d[i]["shaders_types"];
			auto&& array_shaders_types = shaders_types.GetArray();

			std::vector<std::string> shadersTypes;
			for (auto&& s_type : array_shaders_types)
				shadersTypes.push_back(s_type.GetString());

			rapidjson::Value& type = d[i]["type"];
			rapidjson::Value& path = d[i]["path"];
			rapidjson::Value& depth_test = d[i]["depth_test"];
			rapidjson::Value& unique = d[i]["unique"];

			std::string s_dt = depth_test.GetString();
			bool s_uniq = unique.GetBool();

			vector_pairs[type.GetString()] = { path.GetString(), shadersTypes, s_uniq, { (s_dt == "true" ? true : false) } };
		}

		pair_File.close();
	}
	else
	{
		std::cout << "ERROR::ResourceManager: " << path << " file not sucessfully read" << std::endl;
	}
}


void ResourceManager::loadShaderPairs(const char* path, std::map<std::string, shader_path>& shader_pairs)
{
	std::ifstream pair_File;
	pair_File.open(path);

	if (pair_File)
	{
		std::string pairs;
		std::stringstream stream_pairs;
		stream_pairs << pair_File.rdbuf();
		pairs = stream_pairs.str();

		rapidjson::Document d;
		d.Parse(pairs.c_str());

		for (unsigned int i = 0; i < d.Size(); i++)
		{
			rapidjson::Value& type = d[i]["type"];
			rapidjson::Value& path_vertex = d[i]["path_vertex"];
			rapidjson::Value& path_fragment = d[i]["path_fragment"];

			shader_pairs[type.GetString()] = { path_vertex.GetString(), path_fragment.GetString() };
		}

		pair_File.close();
	}
	else
	{
		std::cout << "ERROR::ResourceManager: " << path << " file not sucessfully read" << std::endl;
	}
}

void ResourceManager::loadModels(uint32_t number, std::vector<ObjectAttributes>& objectsAttrib, uint32_t& progress)
{
	std::string levelData = readFile(m_listPathLevels[number]);

	rapidjson::Document d;
	d.Parse(levelData.c_str());

	uint32_t progress_step = (100 / d.Size()) + 1;

	for (unsigned int i = 0; i < d.Size(); i++)
	{
		rapidjson::Value& type = d[i]["models_type"];
		auto types = type.GetArray();
		
		std::vector<size_t> indexes;
		for (auto&& type : types)
		{
			uint32_t model_index = getModelIndex(type.GetString());
			std::vector<uint32_t> shaders_indices = getShadersIndices(type.GetString());

			m_modelIndex_shadersIndices.push_back({ model_index, shaders_indices });
			indexes.push_back((m_modelIndex_shadersIndices.size() - 1));
		}

		rapidjson::Value& posx = d[i]["posx"];
		rapidjson::Value& posy = d[i]["posy"];
		rapidjson::Value& posz = d[i]["posz"];
		rapidjson::Value& scale = d[i]["scale"];
		rapidjson::Value& exsist = d[i]["exsist"];
		rapidjson::Value& optionalProperties = d[i]["optionalProperties"];

		rapidjson::Value& object_type = d[i]["object_type"];
		std::string str = object_type.GetString();
		//m_complete_models[str] = indexes;
		m_map_complete_models[str] = indexes;
		m_complete_models.push_back(indexes);

		ObjectAttributes temp{  (m_complete_models.size() - 1),
								object_type.GetString(),
								posx.GetString(), 
								posy.GetString(), 
								posz.GetString(), 
								scale.GetString(), 
								exsist.GetString(),
								optionalProperties.GetString() };

		objectsAttrib.push_back(temp);

		progress += progress_step;
	}
}

uint32_t ResourceManager::getModelIndex(const std::string& type_of_model)
{
	auto findedModel = std::find_if(m_models.begin(), m_models.end(),
									[&type_of_model](const std::tuple<std::string, Model, rendering_parameters>& x) { return std::get<0>(x) == type_of_model; });

	if (m_models_type_path[type_of_model].unique)
		findedModel = m_models.end();

	unsigned int model_index = -1;
	if (findedModel == m_models.end()) // if model not loaded
	{
		Model model(m_models_type_path[type_of_model].path.c_str());
		m_models.push_back({ type_of_model, model, m_models_type_path[type_of_model].rend_params });
		model_index = m_models.size() - 1;
	}
	else
	{
		model_index = findedModel - m_models.begin();
	}

	return model_index;
}

std::vector<uint32_t> ResourceManager::getShadersIndices(const std::string& type_of_model)
{
	std::vector<uint32_t> shaders_indices;

	for (auto&& type_of_shader : m_models_type_path[type_of_model].shaders_types)
	{
		auto findedShader = std::find_if(m_shaders.begin(), m_shaders.end(),
										 [&type_of_shader](const std::pair<std::string, Shader>& x) { return x.first == type_of_shader; });

		unsigned int shader_index = -1;
		if (findedShader == m_shaders.end()) // if shader not loaded
		{
			Shader shader((m_shaders_type_path[type_of_shader].vertex).c_str(),
				(m_shaders_type_path[type_of_shader].fragment).c_str());

			m_shaders.push_back({ type_of_shader, shader });
			shader_index = m_shaders.size() - 1;
		}
		else
		{
			shader_index = findedShader - m_shaders.begin();
		}

		shaders_indices.push_back(shader_index);
	}

	return shaders_indices;
}

std::string ResourceManager::readFile(const std::string& path)
{
	std::ifstream levelFile;
	std::string data;

	try
	{
		levelFile.open(path);
		std::stringstream levelDataStream;
		levelDataStream << levelFile.rdbuf();
		data = levelDataStream.str();
		levelFile.close();
	}
	catch (std::ifstream::failure)
	{
		std::cout << "ResourceManager load level is failed" << std::endl;
	}

	return data;
}

GLuint ResourceManager::createObject(std::vector<GLfloat> vertices, const std::string& vertexPath, const std::string& fragmentPath)
{
	Shader shader(vertexPath.c_str(), fragmentPath.c_str());
	m_shaders.push_back({ "AREA", shader });
	uint32_t shaderIndex = m_shaders.size() - 1;

	std::vector<GLuint> indices(vertices.size() / 3);
	for (uint32_t i = 0; i < indices.size(); i++) indices[i] = i;

	GLuint VAO, VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), &indices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	ManuallyCreatedObject manObj(VAO, VBO, EBO, 1.0f, glm::vec3(0.0f, 0.0f, 0.0f), vertices);
	m_manuallyCreaatedObjects.push_back(manObj);

	m_manCrObj_indexs.push_back({ m_manuallyCreaatedObjects.size() - 1, shaderIndex });

	return m_manCrObj_indexs.size() - 1;
}

void ResourceManager::updateVBO(uint32_t id, std::vector<GLfloat>& vertices)
{
	GLuint VBO = m_manuallyCreaatedObjects[id].m_VBO;
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
}

void ResourceManager::clear()
{
	m_shaders.clear();
	m_models.clear();
	m_manuallyCreaatedObjects.clear();
	m_modelIndex_shadersIndices.clear();
	m_complete_models.clear();
	m_manCrObj_indexs.clear();
}