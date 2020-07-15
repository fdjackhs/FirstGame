#include <iostream>

#include "Planet.h"

#include <math.h>

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

	m_healthPoints = 1.0f;

	if (m_fraction == "NEUTRAL")
	{
		m_indexes_of_displayd_models.push_back(2);
		m_healthPoints = 0.0f;
	}


	m_position = pos;
	m_scale = scale;
	m_optionalProperties = opt_prop;
	m_selected = false;
	m_type = "Planet";

	m_reloading = 0.0f;
	m_reloadingTime = 1.0f;
	
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