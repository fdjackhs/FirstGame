#include <iostream>

#include "Unit.h"

#include <math.h>

#define pi 3.141592
#define pi2 2 * pi

Unit::Unit(const std::vector<unsigned int>& ID, const glm::vec3& position, float scale, const std::string& opt_prop, const std::string fraction)
{
	m_modelIDs = ID;
	m_position = position;
	m_targetPos = m_position;
	m_scale = scale;
	m_optionalProperties = opt_prop;

	m_type = "Unit";
	m_selected = false;
	m_state = "rest";
	m_speed = 10.00000f;

	m_gravityOffset = glm::vec3(0.0f, 0.0f, 0.0f);

	m_physics_counter = 0;

	m_fraction = fraction; //color

	m_radius = 1.8f / 2;
	m_annihilated = false;
}

Unit::Unit(const std::vector<unsigned int>& ID, const glm::vec3& position, const glm::vec3& target, const std::string fraction)
{
	m_modelIDs = ID;
	m_position = position;
	m_targetPos = target;
	m_scale = 1.0f;
	m_optionalProperties = "";

	m_type = "Unit";
	m_selected = false;
	m_state = "move";
	m_speed = 10.00000f;

	m_gravityOffset = glm::vec3(0.0f, 0.0f, 0.0f);

	m_physics_counter = 0;

	m_fraction = fraction; //color

	m_radius = 1.8f / 2;
	m_annihilated = false;
}

void Unit::action(float deltaTime)
{
	if (m_state == "rest")
	{
		m_position += m_gravityOffset * deltaTime;
	}
	else if (m_state == "move" || m_state == "update")
	{
		glm::vec3 move_vector = glm::normalize(m_targetPos - m_position);
		m_position += move_vector * m_speed * deltaTime;
	}
	else
	{
		while (false);
	}


	if (glm::distance(m_position, m_targetPos) < 0.1f)
	{
		m_state = "rest";
	}
}

glm::vec3 Unit::GetPosition() const
{
	return m_position;
}