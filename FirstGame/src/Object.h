#pragma once
#include "glm/vec3.hpp"

#include "Model.h"

class Object
{
public:
	Object(const glm::vec3& pos);
	~Object();

	glm::vec3 coords;
	std::shared_ptr<Model> model;
};