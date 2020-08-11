#include <iostream>

#include "Planet.h"

#define pi 3.141592
#define pi2 2 * pi

Planet::Planet(const std::vector<unsigned int>& ID, const glm::vec3& pos, float scale, const std::string& opt_prop, Game* game, const std::string fraction)
{
	m_fraction = fraction; //color

	m_modelIDs = ID;
	if (m_fraction == "RED")
		m_indexes_of_displayd_models.push_back(0);
	if (m_fraction == "BLUE")
		m_indexes_of_displayd_models.push_back(1);

	if (m_fraction == "NEUTRAL")
		m_indexes_of_displayd_models.push_back(2);

	//HP scale always last
	m_indexes_of_displayd_models.push_back(m_modelIDs.size() - 1);
	m_indexOfHPscaleRM = RenderEngine::resourceManager.m_modelIndex_shadersIndices[m_modelIDs[m_modelIDs.size() - 1]].first;

	m_position = pos;
	m_scale = scale;
	m_optionalProperties = opt_prop;

	m_type = PLANET;

	m_reloading = 0.0f;
	m_reloadingTime = 1.0f;
	
	m_healthPoints = 100.0f;

	m_level = 1;

	m_gameState = game;

	m_radius = 9.0f;
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

void Planet::plusOne()
{
	if (m_level < s_max_level)
	{
		m_healthPoints += 1.0f;

		if (m_healthPoints == 200.0f)
		{
			m_healthPoints = 100.0f;
			m_level += 1;

			m_scale *= 1.25f;
			m_radius *= 1.25f;
		}
	}

	updateHPscaleVertices();
}

void Planet::minusOne(const std::string& unitFraction)
{
	m_healthPoints -= 1.0f;

	if (m_healthPoints == 0.0f)
	{
		m_healthPoints = 100.0f;

		if (m_fraction == "NEUTRAL")
		{
			m_fraction = unitFraction;
			m_level = 1;

			// Then fraction will be more than 2, 
			// need find right index of model
			m_indexes_of_displayd_models.clear();
			m_indexes_of_displayd_models.push_back(0);
		}
		else
		{
			m_fraction = "NEUTRAL";
			m_level = 1;

			m_indexes_of_displayd_models.clear();
			m_indexes_of_displayd_models.push_back(2);
		}

		m_scale = 1.0f;
		m_radius = 9.0f;
	}

	updateHPscaleVertices();
}


void Planet::updateHPscaleVertices()
{
	//for (auto&& index : m_indexes_of_displayd_models)
	{
		Model tempModel = std::get<1>(RenderEngine::resourceManager.m_models[m_indexOfHPscaleRM]);

		std::vector<glm::vec3> new_vertices;
		std::vector<glm::vec3> new_texcoords;

		new_vertices.push_back({ 0.0f, 0.0f, 0.0f }); //center
		new_texcoords.push_back({ 0.0f, 0.0f, 0.0f });

		float step = 360.0f / 100.0f;
		float top = (m_healthPoints - 100.0f) * step;

		for (float angle = 0.0f; angle <= top; angle += step)
		{
			float x = cos(glm::radians(angle)) * (m_radius * 1.15); 
			float z = sin(glm::radians(angle)) * (m_radius * 1.15);
			new_vertices.push_back({ x, 0.0f, z });


			x = sin(glm::radians(angle)) * 0.5f;
			z = cos(glm::radians(angle)) * 0.5f;
			new_texcoords.push_back({ x, 0.0f, z });
		}

		tempModel.meshes[0].vertices.clear();
		tempModel.meshes[0].indices.clear();

		unsigned int ind = 2;
		for (unsigned int i = 0; i < new_vertices.size(); i++, ind++)
		{
			Vertex vertex;
			vertex.Position = new_vertices[i];
			vertex.TexCoords = new_texcoords[i];
			tempModel.meshes[0].vertices.push_back(vertex);

			tempModel.meshes[0].indices.push_back(0);
			tempModel.meshes[0].indices.push_back(ind - 1);
			tempModel.meshes[0].indices.push_back(ind);
		}

		tempModel.meshes[0].updateMesh();
		std::get<1>(RenderEngine::resourceManager.m_models[m_indexOfHPscaleRM]) = tempModel;
	}
}