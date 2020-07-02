#include "ResourceManager.h"

ResourceManager::ResourceManager()
{
	ResourceManager::loadListPathLevels();
	loadShaderPairs("../FirstGame/Resources/ResourceManager/shaders_type_path.txt", shaders_type_path);
	loadModelPairs("../FirstGame/Resources/ResourceManager/models_type_path.txt", models_type_path);
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
			listPathLevels.push_back(path);

		levelPathsFile.close();
	}
	else
	{
		std::cout << "ERROR::ResourceManager: list_path_levels.txt file not sucessfully read" << std::endl;
	}
}

void ResourceManager::loadModelPairs(const char* path, std::map<std::string, std::string>& vector_pairs)
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
			rapidjson::Value& path = d[i]["path"];
			rapidjson::Value& shaderType = d[i]["shader_type"];

			vector_pairs[type.GetString()] = path.GetString();
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

void ResourceManager::loadLevel(unsigned int number, std::vector<ObjectAttributes>& objectsAttrib)
{
	m_stencilShader = std::make_shared<Shader>("../FirstGame/Resources/shaders/1.stencil_shader.vs", "../FirstGame/Resources/shaders/1.stencil_shader.fs");
	m_areaShader = std::make_shared<Shader>("../FirstGame/Resources/shaders/1.area_shader.vs", "../FirstGame/Resources/shaders/1.area_shader.fs");

	std::string levelData;
	std::ifstream levelFile;

	try
	{
		levelFile.open(listPathLevels[number]);
		std::stringstream levelDataStream;
		levelDataStream << levelFile.rdbuf();
		levelData = levelDataStream.str();
		levelFile.close();
	}
	catch (std::ifstream::failure)
	{
		std::cout << "ResourceManager load level is failed" << std::endl;
	}

	rapidjson::Document d;
	d.Parse(levelData.c_str());

	for (unsigned int i = 0; i < d.Size(); i++)
	{
		rapidjson::Value& type = d[i]["models_type"];
		auto types = type.GetArray();
		
		std::vector<size_t> indexes;
		for (auto&& type : types)
		{
			auto findedModel = std::find_if(
				models.begin(), models.end(),
				[&type](const std::pair<std::string, Model>& x) { return x.first == type.GetString(); });

			unsigned int model_index = -1;
			if (findedModel == models.end()) // if model not loaded
			{
				Model model((models_type_path[type.GetString()]).c_str());
				models.push_back({ type.GetString(), model });
				model_index = models.size() - 1;
			}
			else
			{
				model_index = findedModel - models.begin();
			}


			auto findedShader = std::find_if(
				shaders.begin(), shaders.end(),
				[&type](const std::pair<std::string, Shader>& x) { return x.first == type.GetString(); });

			unsigned int shader_index = -1;
			if (findedShader == shaders.end()) // if shader not loaded
			{
				Shader shader((shaders_type_path[type.GetString()].vertex).c_str(), (shaders_type_path[type.GetString()].fragment).c_str());
				shaders.push_back({ type.GetString(), shader });
				shader_index = shaders.size() - 1;
			}
			else
			{

				shader_index = findedShader - shaders.begin();
			}

			modelIndex_shaderIndex.push_back({ model_index, shader_index });
			indexes.push_back((modelIndex_shaderIndex.size() - 1));
		}


		rapidjson::Value& posx = d[i]["posx"];
		rapidjson::Value& posy = d[i]["posy"];
		rapidjson::Value& posz = d[i]["posz"];
		rapidjson::Value& scale = d[i]["scale"];
		rapidjson::Value& optionalProperties = d[i]["optionalProperties"];

		ObjectAttributes temp{  indexes,
								posx.GetString(), 
								posy.GetString(), 
								posz.GetString(), 
								scale.GetString(), 
								optionalProperties.GetString() };

		objectsAttrib.push_back(temp);
	}
}

GLuint ResourceManager::createObject(GLfloat* vertices, const std::string& vertexPath, const std::string& fragmentPath, GLuint& vao)
{
	Shader shader(vertexPath.c_str(), fragmentPath.c_str());
	shaders.push_back({ "", shader });
	uint32_t shaderIndex = shaders.size() - 1;

	GLuint indices[80];
	for (uint32_t i = 0; i < 80; i++) indices[i] = i;

	GLuint VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 240, vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	m_manuallyCreaatedObjects.push_back({ VAO, shaderIndex });

	vao = VAO;
	return VBO;
}

void ResourceManager::updateVBO(uint32_t vbo, GLfloat* vertices)
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 240, vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
}