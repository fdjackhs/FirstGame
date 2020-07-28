#include "Button.h"


Button::Button(const std::vector<unsigned int>& ID, const glm::vec3& pos, float scale, const std::string& opt_prop)
{
	m_modelIDs = ID;
	m_indexes_of_displayd_models.push_back(0);

	m_position = pos;
	m_scale = scale;
	m_optionalProperties = opt_prop;
	m_type = BUTTON;
	m_isPressed = false;

	{
		int tempIndex = RenderEngine::resourceManager.modelIndex_shadersIndices[m_modelIDs[m_indexes_of_displayd_models[0]]].first;
		Model temp = std::get<1>(RenderEngine::resourceManager.models[tempIndex]);

		std::vector<std::pair<float, float>> vertices;

		for (int i = 0; i < 4; i++)
		{
			float x = temp.meshes[0].vertices[i].Position.x;
			float y = temp.meshes[0].vertices[i].Position.y;

			vertices.push_back({ x, y });
		}

		std::sort(vertices.begin(), vertices.end(), [](const std::pair<float, float>& lhs, const std::pair<float, float>& rhs) { return lhs.first < rhs.first; });
		float x = vertices[vertices.size() - 1].first - vertices[0].first;

		std::sort(vertices.begin(), vertices.end(), [](const std::pair<float, float>& lhs, const std::pair<float, float>& rhs) { return lhs.second < rhs.second; });
		float y = vertices[vertices.size() - 1].second - vertices[0].second;

		m_size = glm::vec2(x, y) * m_scale;
	}

	m_left_top_corner.x = m_position.x - (m_size.x / 2.0f);
	m_left_top_corner.y = m_position.y + (m_size.y / 2.0f);
	
	//transform to interval 0.0f - 1.0f (now it's -1.0 - 1.0f)
	m_left_top_corner.x = (m_left_top_corner.x + 1.0f) / 2.0f;
	m_left_top_corner.y = (1.0f - m_left_top_corner.y) / 2.0f;

	//size need to transform too
	m_size *= 0.5f;

	//m_left_top_corner.x = m_left_top_corner.x * RenderEngine::SCREEN.x;
	//m_left_top_corner.y = m_left_top_corner.y * RenderEngine::SCREEN.y;
}

bool Button::checkButtonPress(const glm::vec2& clickCoords)
{
	if (clickCoords.x >= m_left_top_corner.x && clickCoords.x <= m_left_top_corner.x + m_size.x &&
		clickCoords.y >= m_left_top_corner.y && clickCoords.y <= m_left_top_corner.y + m_size.y)
	{
		return true;
	}

	return false;
}

void Button::setButtonDown()
{
	m_indexes_of_displayd_models.clear();
	m_indexes_of_displayd_models.push_back(1);
}

void Button::setButtonUp()
{
	m_indexes_of_displayd_models.clear();
	m_indexes_of_displayd_models.push_back(0);
}