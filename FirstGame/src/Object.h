#pragma once
#include "glm/vec3.hpp"

#include "Model.h"

class Object
{
public:
	Object(unsigned int ID, const glm::vec3& pos, float scale, const std::string& opt_prop);
	~Object();

	unsigned int modelID;
	glm::vec3 position;
	float scale;
	std::string optionalProperties;
};