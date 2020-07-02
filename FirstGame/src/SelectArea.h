#pragma once
#include <vector>

#include <glm/glm.hpp>

#define pi 3.141592f
#define pi2 2.0f * pi

struct select_area
{
	bool firstTime;
	float radius;
	float radiusInWorldCoords;

	glm::vec2 areaStartPosition;
	glm::vec2 center;

	glm::vec3 centerInWorldCoords;

	//for drawing
	//-----------
	static const uint32_t size = 240;
	GLfloat vertices[size];
	GLuint vao, vbo;

	void updateVertices()
	{
		for (unsigned int i = 0; i < (size / 3); i++)
		{
			float angle = pi2 * float(i) / (size / 3);

			vertices[i * 3 + 0]	= radiusInWorldCoords * (float)cos(angle);
			vertices[i * 3 + 2] = radiusInWorldCoords * (float)sin(angle);
		}
	}
};