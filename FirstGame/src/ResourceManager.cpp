#include "ResourceManager.h"

ResourceManager::ResourceManager()
{
	ResourceManager::loadListPathLevels();
	loadShaderPairs("../FirstGame/Resources/ResourceManager/shaders_type_path.txt", shaders_type_path);
	loadPairs("../FirstGame/Resources/ResourceManager/models_type_path.txt", models_type_path);
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

void ResourceManager::loadPairs(const char* path, std::map<std::string, std::string>& vector_pairs)
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
		rapidjson::Value& type = d[i]["type"];

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

		modelIndex_shaderIndex.push_back({ model_index, shader_index});

		rapidjson::Value& posx = d[i]["posx"];
		rapidjson::Value& posy = d[i]["posy"];
		rapidjson::Value& posz = d[i]["posz"];
		rapidjson::Value& scale = d[i]["scale"];
		rapidjson::Value& optionalProperties = d[i]["optionalProperties"];

		ObjectAttributes temp{ (modelIndex_shaderIndex.size() - 1), 
								posx.GetString(), 
								posy.GetString(), 
								posz.GetString(), 
								scale.GetString(), 
								optionalProperties.GetString() };

		objectsAttrib.push_back(temp);
	}
}
