#pragma once
#include <vector>

#include <glm/glm.hpp>

#include "RenderEngine.h"
#include "Object.h"


#define pi 3.141592f
#define pi2 2.0f * pi

class select_area
{
public:
	bool firstTime;
	float radius;
	uint32_t ID;

	glm::vec2 areaStartPosition;
	glm::vec3 m_position;

	/**/std::vector<GLfloat> vertices;

	select_area(uint32_t id);
	void updateVertices();
};