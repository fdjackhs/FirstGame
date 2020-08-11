#pragma once
#include <string>

struct ObjectAttributes
{
	uint32_t id;

	std::string object_type;

	std::string posx;
	std::string posy;
	std::string posz;

	std::string scale;

	std::string exsist;

	std::string optionalProperties;
};