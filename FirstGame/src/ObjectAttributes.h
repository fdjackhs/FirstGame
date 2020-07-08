#pragma once
#include <string>

struct ObjectAttributes
{
	std::vector<unsigned int> IDs; //for resource manager

	std::string posx;
	std::string posy;
	std::string posz;

	std::string scale;

	std::string exsist;

	std::string optionalProperties;
};