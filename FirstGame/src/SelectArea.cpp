#include "SelectArea.h"

select_area::select_area(uint32_t id)
{
	m_ID = id;
	m_exsist = false;
	m_fixed = false;
	m_areaStartPosition = glm::vec3(0.0f, 0.0f, 0.0f);
	m_vertices = move(std::vector<GLfloat>(240));
}

void select_area::updateVertices()
{
	for (uint32_t i = 0; i < (m_vertices.size() / 3); i++)
	{
		float angle = glm::radians(float(i) * (360.0f / float(m_vertices.size() / 3)));

		m_vertices[i * 3 + 0] = m_radius * (float)cos(angle);
		m_vertices[i * 3 + 1] = 0.0f;
		m_vertices[i * 3 + 2] = m_radius * (float)sin(angle);
	}
}

void select_area::updateArea(const glm::vec2& cursorCoords)
{
	if (!m_exsist || (m_exsist && m_fixed))
	{
		m_areaStartPosition = cursorCoords;
		m_exsist = true;
		m_fixed = false;
	}

	glm::vec2 diff = glm::vec2(cursorCoords.x - m_areaStartPosition.x, cursorCoords.y - m_areaStartPosition.y);
	glm::vec2 center = m_areaStartPosition + glm::vec2(diff.x / 2, diff.y / 2);

	glm::vec3 centerInWorldCoords = RenderEngine::cursorCoordToWorldCoords({ center.x, center.y });
	glm::vec3 borderInWorldCoords = RenderEngine::cursorCoordToWorldCoords({ cursorCoords.x, cursorCoords.y });

	uint32_t indexOfarea = RenderEngine::resourceManager.m_manCrObj_indexs[m_ID].first;

	RenderEngine::resourceManager.m_manuallyCreaatedObjects[indexOfarea].m_position = centerInWorldCoords;
	m_position = centerInWorldCoords;
	m_radius = glm::distance(centerInWorldCoords, borderInWorldCoords);

	updateVertices();
	RenderEngine::resourceManager.updateVBO(m_ID, m_vertices);
}

void select_area::collapseArea()
{
	m_radius = 0.0f;
	updateVertices();
	RenderEngine::resourceManager.updateVBO(m_ID, m_vertices);
}