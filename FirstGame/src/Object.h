#pragma once

#include "glm/vec3.hpp"

class Object
{
public:
	Object(const glm::vec3& pos);
	~Object();

	glm::vec3 coords;
	// model
	// draw
	// state
};