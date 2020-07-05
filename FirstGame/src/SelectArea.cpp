#include "SelectArea.h"
#include "RenderEngine.h"

select_area::select_area(uint32_t id)
{
	ID = id;
	firstTime = true;
	areaStartPosition = glm::vec3(0.0f, 0.0f, 0.0f);
	vertices = move(std::vector<GLfloat>(240));
}

void select_area::updateVertices()
{
	for (unsigned int i = 0; i < (vertices.size() / 3); i++)
	{
		float angle = pi2 * float(i) / (vertices.size() / 3);

		vertices[i * 3 + 0] = radius * (float)cos(angle);
		vertices[i * 3 + 2] = radius * (float)sin(angle);
	}
}