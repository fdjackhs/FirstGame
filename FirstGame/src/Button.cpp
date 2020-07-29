#include "Button.h"

Button::Button(const std::vector<unsigned int>& ID, const glm::vec3& pos, float scale, Game* game, const std::string& opt_prop)
{
	m_modelIDs = ID;
	m_indexes_of_displayd_models.push_back(0);

	m_position = pos;
	m_scale = scale;
	m_optionalProperties = opt_prop;
	m_type = BUTTON;
	m_isPressed = false;
	m_gameState = game;
	m_function = nullptr;

	updateButtonVertices();
	calculateSizeOfButton();

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

bool Button::cursorIsOnButton(const glm::vec2& clickCoords)
{
	if (clickCoords.x >= m_left_top_corner.x && clickCoords.x <= m_left_top_corner.x + m_size.x &&
		clickCoords.y >= m_left_top_corner.y && clickCoords.y <= m_left_top_corner.y + m_size.y)
		return true;
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

void Button::updateButtonVertices()
{
	for(auto&& modelId : m_modelIDs)
	{
		int modelIndex = RenderEngine::resourceManager.modelIndex_shadersIndices[modelId].first;
		Model tempModel = std::get<1>(RenderEngine::resourceManager.models[modelIndex]);

		if (!tempModel.verticesUpdated)
		{
			for (auto&& vertex : tempModel.meshes[0].vertices)
				vertex.Position.x *= RenderEngine::getScreenAspectRatio();

			tempModel.meshes[0].updateMesh();
			
			tempModel.verticesUpdated = true;
			std::get<1>(RenderEngine::resourceManager.models[modelIndex]) = tempModel;
		}
	}
}

void Button::calculateSizeOfButton()
{
	//calculate only for first model of button (someday needs to be redone for all models)

	int modelIndex = RenderEngine::resourceManager.modelIndex_shadersIndices[m_modelIDs[m_indexes_of_displayd_models[0]]].first;
	Model tempModel = std::get<1>(RenderEngine::resourceManager.models[modelIndex]);

	std::vector<std::pair<float, float>> vertices;
	for (auto&& vertex : tempModel.meshes[0].vertices)
		vertices.push_back({ vertex.Position.x, vertex.Position.y });
	
	std::sort(vertices.begin(), vertices.end(), [](const std::pair<float, float>& lhs, const std::pair<float, float>& rhs) { return lhs.first < rhs.first; });
	float x_size = vertices[vertices.size() - 1].first - vertices[0].first;

	std::sort(vertices.begin(), vertices.end(), [](const std::pair<float, float>& lhs, const std::pair<float, float>& rhs) { return lhs.second < rhs.second; });
	float y_size = vertices[vertices.size() - 1].second - vertices[0].second;

	m_size = glm::vec2(x_size, y_size) * m_scale;
}

void Button::setCallbackFunc(void(*func)(Game&))
{
	m_function = func;
}

void Button::callCallbackFunction()
{
	if(m_function != nullptr)
		m_function(*m_gameState);
}