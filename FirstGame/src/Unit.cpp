#include <iostream>

#include "Unit.h"

#include <math.h>

#define pi 3.141592
#define pi2 2 * pi

Unit::Unit(const std::vector<unsigned int>& ID, const glm::vec3& position, float scale, const std::string& opt_prop)
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
}

Unit::Unit(const std::vector<unsigned int>& ID, const glm::vec3& position, const glm::vec3& target)
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
}

void Unit::action(float deltaTime)
{
	if (m_state == "rest")
	{
		m_position += m_gravityOffset * deltaTime;
	}
	else if (m_state == "move")
	{
		glm::vec3 move_vector = glm::normalize(m_targetPos - m_position);
		m_position += move_vector * m_speed * deltaTime;
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