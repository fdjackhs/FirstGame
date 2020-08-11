#include "Label.h"
#include "RenderEngine.h"

Label::Label() 
{
	m_visible = false;
	m_label = "";
	m_position = glm::vec3(0.0f, 0.0f, 0.0f);
	//m_left_top_corner = glm::vec3(0.0f, 0.0f, 0.0f);
	updateLabelVertices();
}

Label::Label(const std::vector<uint32_t>& alphabet, const Var& value, glm::vec3& position, const float& scale) : m_visible(true), m_corresponding_value(value), m_position(position), m_scale(scale)
{
	m_alphabet = alphabet; 
	updateLabelVertices();
}

Label::Label(const std::vector<uint32_t>& alphabet, const std::string& label, glm::vec3& position, const float& scale) : m_visible(true), m_label(label), m_position(position), m_scale(scale)
{
	m_alphabet = alphabet;
	updateLabelVertices();
}


void Label::updateLabel()
{
	std::string str = m_corresponding_value.toString();
	str = str.substr(0, 4);

	m_value.clear();

	for (auto&& ch : str)
	{
		// push index of char IN static m_alphabet
		if (ch == '.')
			m_value.push_back(11);
		else if (ch == ',')
			m_value.push_back(10);
		else
			m_value.push_back(uint32_t(ch - '0'));
	}
}


void Label::updateLabelVertices()
{
	std::vector<uint32_t> modelIDs = RenderEngine::resourceManager.m_map_complete_models["LABEL"];
	for (auto&& modelId : modelIDs)
	{
		int modelIndex = RenderEngine::resourceManager.m_modelIndex_shadersIndices[modelId].first;
		Model tempModel = std::get<1>(RenderEngine::resourceManager.m_models[modelIndex]);

		if (!tempModel.verticesUpdated)
		{
			for (auto&& vertex : tempModel.meshes[0].vertices)
				vertex.Position.x *= RenderEngine::getScreenAspectRatio();

			tempModel.meshes[0].updateMesh();

			tempModel.verticesUpdated = true;
			std::get<1>(RenderEngine::resourceManager.m_models[modelIndex]) = tempModel;
		}
	}
}