#include "Unit.h"

#include <math.h>

#define pi 3.141592
#define pi2 2 * pi

Unit::Unit(const std::vector<unsigned int>& ID, const glm::vec3& pos, float scale, const std::string& opt_prop)
{
	//Object(ID, pos, scale, opt_prop);

	m_modelIDs = ID;
	m_position = pos;
	m_targetPos = m_position;
	m_scale = scale;
	m_optionalProperties = opt_prop;

	m_selected = false;
	m_state = "rest";
	m_speed = 1.00000f;
	  
	m_front = float((rand() * 10000) % (31415 * 2)) / 10000.0f;
	m_actualPosition = m_position;
}

void Unit::action(float deltaTime)
{
	if (glm::distance(m_position, m_targetPos) > 0.01f)
		m_state = "move";
	else
		m_state = "rest";


	//glm::vec3(sin(angle), 0.0f, cos(angle));
	if (m_state == "rest")
	{
		/*
		float left = m_front - 1.0f;
		float right = m_front + 1.0f;

		glm::vec2 point1(m_actualPosition.x + sin(left), m_actualPosition.z + cos(left));
		glm::vec2 point2(m_actualPosition.x + sin(right), m_actualPosition.z + cos(right));

		float dist1 = glm::distance(point1, glm::vec2(m_actualPosition.x, m_actualPosition.z));
		float dist2 = glm::distance(point2, glm::vec2(m_actualPosition.x, m_actualPosition.z));

		glm::vec2 nearPoint = dist1 > dist2 ? point1 : point2;
		glm::vec2 farPoint  = dist1 < dist2 ? point1 : point2;

		float koef = glm::distance(m_position, m_actualPosition) / 1.0f;

		if (rand() % 100 > koef * 100)
			m_target = glm::vec3(nearPoint.x, 0.0f, nearPoint.y);
		else
			m_target = glm::vec3(farPoint.x, 0.0f, farPoint.y);



		m_actualPosition += m_target * m_speed;
		*/

		//m_front += 0.01f;
		//m_actualPosition = glm::vec3(m_position.x + sin(m_front), 0.0f, m_position.z + cos(m_front));
		


	}
	else if (m_state == "move")
	{
		glm::vec3 move_vector = glm::normalize(m_targetPos - m_position);

		m_position += move_vector * m_speed * deltaTime;

		while (false);
		//m_position = m_actualPosition;
	}
	//m_actualPosition = m_position;
}

glm::vec3 Unit::GetPosition() const
{
	//return m_actualPosition;
	return m_position;
}