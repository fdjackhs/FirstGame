#pragma once
#include "glm/vec3.hpp"

#include "Model.h"
#include "ModelTypes.h"

class Object
{
public:
	Object(const glm::vec3& pos, const std::string& pathToModel);
	~Object();

	glm::vec3 coords;
	//std::shared_ptr<Model> model;
};