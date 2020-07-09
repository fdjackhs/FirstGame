#include <iostream>

#include "Planet.h"

#include <math.h>

Planet::Planet(const std::vector<unsigned int>& ID, const glm::vec3& pos, float scale, const std::string& opt_prop, Game* game, const std::string fraction)
{
	m_modelIDs = ID;
	m_position = pos;
	m_targetPos = m_position;
	m_scale = scale;
	m_optionalProperties = opt_prop;
	m_selected = false;
	m_type = "Planet";

	m_reloading = 0.0f;

	m_gameState = game;

	m_fraction = fraction; //color
}

void Planet::action(float deltaTime)
{
	m_reloading += deltaTime;

	if (m_reloading >= 1.0f)
	{
		m_reloading -= 1.0f;

		
		glm::vec3 target = m_position;

		float angle = float((rand() * 10000) % (31415 * 2)) / 10000.0f;

		target.x = m_position.x + sin(angle) * 20.0f;
		target.y = 0.0f;
		target.z = m_position.z + cos(angle) * 20.0f;
		
		if (m_fraction == "RED")
			m_gameState->createUnit("RED_UNIT", m_position, target, "RED");

		if (m_fraction == "BLUE")
			m_gameState->createUnit("BLUE_UNIT", m_position, target, "BLUE");
	}
}

glm::vec3 Planet::GetPosition() const
{
	return m_position;
}