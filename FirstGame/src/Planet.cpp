#include <iostream>

#include "Planet.h"

#define pi 3.141592f
#define pi2 2.0f * pi

Planet::Planet(const std::vector<unsigned int>& ID, const glm::vec3& pos, float scale, const std::string& opt_prop, Game* game, const std::string fraction)
{
	m_fraction = fraction; //color
	m_modelIDs = ID;

	for (uint32_t i = 0; i < m_modelIDs.size(); i++)
	{
		if (std::get<0>(RenderEngine::resourceManager.m_models[RenderEngine::resourceManager.m_modelIndex_shadersIndices[m_modelIDs[i]].first]) == "CAPTURE_SCALE")
		{
			m_Real_IndexOfCaputreScaleRM = RenderEngine::resourceManager.m_modelIndex_shadersIndices[m_modelIDs[i]].first;
		}
		if (std::get<0>(RenderEngine::resourceManager.m_models[RenderEngine::resourceManager.m_modelIndex_shadersIndices[m_modelIDs[i]].first]) == "UPGRADE_SCALE")
		{
			m_Real_IndexOfUpgradeScaleRM = RenderEngine::resourceManager.m_modelIndex_shadersIndices[m_modelIDs[i]].first;
		}
	}

	determineColor();

	m_position = pos;
	m_scale = scale;
	m_optionalProperties = opt_prop;

	m_type = PLANET;

	m_reloading = 0.0f;
	m_reloadingTime = 1.0f;

	m_upgradePoints = 0.0f;
	m_capturePoints = 0.0f;

	m_level = 1.0f;

	m_gameState = game;

	m_selected = false;

	m_radius = 10.0f;
}

void Planet::action(float deltaTime)
{
	if (m_fraction != "NEUTRAL")
	{
		m_reloading += deltaTime;

		float base_angle = float((rand() * 10000) % (31415 * 2)) / 10000.0f;
		float angle_stride = float(pi2) / m_level;

		if (m_reloading >= m_reloadingTime)
		{
			m_reloading -= m_reloadingTime;

			for (uint32_t i = 0; i < m_level; i++)
			{
				glm::vec3 target = m_position;


				target.x = m_position.x + sin(base_angle + (angle_stride * i)) * (10.0f + float(rand() % 20));
				target.y = 0.0f;
				target.z = m_position.z + cos(base_angle + (angle_stride * i)) * (10.0f + float(rand() % 20));

				if (m_fraction == "RED")
					m_gameState->createUnit("RED_UNIT", m_position, target, "RED");

				if (m_fraction == "BLUE")
					m_gameState->createUnit("BLUE_UNIT", m_position, target, "BLUE");
			}
		}
	}
}

glm::vec3 Planet::GetPosition() const
{
	return m_position;
}

void Planet::determineColor()
{
	m_indexes_of_displayd_models.clear();

	//set color
	for (uint32_t i = 0; i < m_modelIDs.size(); i++)
	{
		if (std::get<0>(RenderEngine::resourceManager.m_models[RenderEngine::resourceManager.m_modelIndex_shadersIndices[m_modelIDs[i]].first]) == m_fraction + "_PLANET")
		{
			m_indexes_of_displayd_models.push_back(i);
		}
	}

	//set capture scale
	for (uint32_t i = 0; i < m_modelIDs.size(); i++)
	{
		if (std::get<0>(RenderEngine::resourceManager.m_models[RenderEngine::resourceManager.m_modelIndex_shadersIndices[m_modelIDs[i]].first]) == "CAPTURE_SCALE")
		{
			m_indexes_of_displayd_models.push_back(i);
		}
	}
	//set upgrade scale
	for (uint32_t i = 0; i < m_modelIDs.size(); i++)
	{
		if (std::get<0>(RenderEngine::resourceManager.m_models[RenderEngine::resourceManager.m_modelIndex_shadersIndices[m_modelIDs[i]].first]) == "UPGRADE_SCALE")
		{
			m_indexes_of_displayd_models.push_back(i);
		}
	}
}

void Planet::plusOne()
{
	if (m_level < s_max_level)
	{
		if (m_capturePoints == 0.0f)
		{
			m_upgradePoints += 1.0f;

			if (m_upgradePoints == 100.0f)
			{
				m_upgradePoints = 0.0f;
				m_level += 1;
			}
		}
		else
		{
			m_capturePoints -= 1.0f;
		}
	}

	updateScaleVertices();
}


void Planet::minusOne(const std::string& unitFraction)
{
	if (m_upgradePoints == 0.0f)
	{
		m_capturePoints += 1.0f;

		if (m_capturePoints == 100.0f)
		{
			if (m_fraction == "NEUTRAL")
				m_fraction = unitFraction;
			else if (m_fraction != "NEUTRAL")
				m_fraction = "NEUTRAL";

			determineColor();

			m_level = 1;
			m_radius = 10.0f;
			m_capturePoints = 0.0f;
		}
	}
	else
	{
		m_upgradePoints -= 1.0f;
	}

	updateScaleVertices();
}


void Planet::updateScaleVertices()
{
	int32_t indexOfScale = -1;
	float topBorder;

	bool flag = false;
	if (m_capturePoints > 0.0f)
	{
		indexOfScale = m_Real_IndexOfCaputreScaleRM;
		topBorder = m_capturePoints;
		flag = true;
	}
	else if (m_upgradePoints > 0.0f)
	{
		if (flag)
			while (false);
		indexOfScale = m_Real_IndexOfUpgradeScaleRM;
		topBorder = m_upgradePoints;
	}

	if (indexOfScale != -1)
	{
		Model tempModel = std::get<1>(RenderEngine::resourceManager.m_models[indexOfScale]);

		std::vector<glm::vec3> new_vertices;
		new_vertices.push_back({ 0.0f, 0.0f, 0.0f }); //center

		float step = 360.0f / 100.0f;
		float top = (topBorder) * step;

		for (float angle = 0.0f; angle <= top; angle += step)
		{
			float x = -sin(glm::radians(angle) + pi) * (m_radius + 2.0f);
			float z = cos(glm::radians(angle) + pi) * (m_radius + 2.0f);
			new_vertices.push_back({ x, 0.0f, z });
		}

		tempModel.meshes[0].vertices.clear();
		tempModel.meshes[0].indices.clear();

		unsigned int ind = 2;
		for (unsigned int i = 0; i < new_vertices.size(); i++, ind++)
		{
			Vertex vertex;
			vertex.Position = new_vertices[i];
			tempModel.meshes[0].vertices.push_back(vertex);

			tempModel.meshes[0].indices.push_back(0);
			tempModel.meshes[0].indices.push_back(ind - 1);
			tempModel.meshes[0].indices.push_back(ind);
		}

		tempModel.meshes[0].updateMesh();
		std::get<1>(RenderEngine::resourceManager.m_models[indexOfScale]) = tempModel;
	}
	else
	{
		Model captureScale = std::get<1>(RenderEngine::resourceManager.m_models[m_Real_IndexOfCaputreScaleRM]);
		Model upgradeScale = std::get<1>(RenderEngine::resourceManager.m_models[m_Real_IndexOfUpgradeScaleRM]);

		captureScale.meshes[0].vertices.clear();
		upgradeScale.meshes[0].vertices.clear();

		captureScale.meshes[0].indices.clear();
		upgradeScale.meshes[0].indices.clear();

		Vertex vertex;
		vertex.Position = glm::vec3(0.0f, 0.0f, 0.0f);

		captureScale.meshes[0].vertices.push_back(vertex);
		upgradeScale.meshes[0].vertices.push_back(vertex); 
			
		captureScale.meshes[0].indices.push_back(0);
		upgradeScale.meshes[0].indices.push_back(0);

		captureScale.meshes[0].updateMesh();
		upgradeScale.meshes[0].updateMesh();

		std::get<1>(RenderEngine::resourceManager.m_models[m_Real_IndexOfCaputreScaleRM]) = captureScale;
		std::get<1>(RenderEngine::resourceManager.m_models[m_Real_IndexOfUpgradeScaleRM]) = upgradeScale;
	}
}

void Planet::select()
{
	if (!m_selected)
	{
		m_selected = true;

		m_indexes_of_displayd_models.clear();
		m_indexes_of_displayd_models.push_back(0);
		m_indexes_of_displayd_models.push_back(1);
	}
}

void Planet::deselect()
{
	if (m_selected)
	{
		m_selected = false;
		m_indexes_of_displayd_models.clear();
		m_indexes_of_displayd_models.push_back(1);
	}
}