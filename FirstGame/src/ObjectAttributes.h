#pragma once
#include <string>

struct ObjectAttributes
{
	unsigned int ID; //for resource manager

	std::string posx;
	std::string posy;
	std::string posz;

	std::string scale;

	std::string optionalProperties;
};