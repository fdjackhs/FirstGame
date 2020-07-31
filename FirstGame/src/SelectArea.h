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
	bool m_exsist;
	bool m_fixed;
	float m_radius;
	uint32_t m_ID;

	glm::vec2 m_areaStartPosition;
	glm::vec3 m_position;

	std::vector<GLfloat> m_vertices;

	select_area(uint32_t id);

	void updateVertices();
	void updateArea(const glm::vec2& cursorCoords);
	void collapseArea();
};