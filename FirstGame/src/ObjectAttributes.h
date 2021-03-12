#pragma once
#include <string>

struct ObjectAttributes
{
	//for create object
	unsigned int id;

	std::string object_type;

	std::vector<std::string> models_type;

	std::string posx;
	std::string posy;
	std::string posz;

	std::string scale;

	std::string exsist;

	std::string optionalProperties;
};