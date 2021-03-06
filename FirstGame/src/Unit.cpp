#include <iostream>

#include "Unit.h"
#include "Utilities.h"
#include "Game.h"

#define pi 3.141592f
#define pi2 2.0f * pi

Unit::Unit(const std::vector<unsigned int>& ID, const glm::vec3& position, float scale, const std::string& opt_prop, const std::string fraction, Game* game)
{
	m_modelIDs = ID;
	m_indexes_of_displayd_models.push_back(1);

	m_position = position;
	m_scale = scale;
	m_optionalProperties = opt_prop;
	m_fraction = fraction; //color
	m_game = game;
	m_targetPos = position;
	m_centerPosition = position;

	m_type = UNIT;
	m_selected = false;
	m_state = "rest";
	m_speed = 10.00000f;
	m_physCounter = 61;
	m_lastAngle = 0.0f;
	m_radius = 1.8f / 2;
	m_annihilated = false;
	m_globCounter = 0.0f;

	m_gravityOffset = glm::vec3(0.0f, 0.0f, 0.0f);
}

void Unit::action(float deltaTime)
{
	if (m_state == "rest")
	{
		//m_physCounter += deltaTime;

		if (m_physCounter > 60.0f)
		{
			m_globCounter++;
			m_physCounter = 0;

			float dist = glm::distance(m_position, m_centerPosition);

			if (dist == 0.0f)
				dist = 0.01f;

			dist = dist / 10.0f;

			if (dist < 1.0f)
			{
				float baseAngle = -pi * dist;
				float randAngle = 0.0f;

				if (rand() % 2)
					randAngle = glm::radians(float(rand() % 10));
				else
					randAngle = -glm::radians(float(rand() % 50));

				m_lastAngle += (baseAngle + randAngle);

				m_currVector.x = sin(m_lastAngle) * 10.0f;
				m_currVector.z = cos(m_lastAngle) * 10.0f;
				m_currVector.y = 0.0f;

				if (m_currVector.x == 0.0f && m_currVector.y == 0.0f)
					m_currVector.x = 1.1f;
			}
			else
			{
				m_currVector = m_centerPosition - m_position;
			}

		}
		else
		{
			glm::vec3 nextPosition = m_position + glm::normalize(m_currVector) * deltaTime;
			if (nextPosition.x > m_game->m_edgesOfMap->left && nextPosition.x < m_game->m_edgesOfMap->right && nextPosition.z > m_game->m_edgesOfMap->bot  && nextPosition.z < m_game->m_edgesOfMap->top)
				m_position = nextPosition;

			//m_position += glm::normalize(m_currVector) * deltaTime;
			m_physCounter++;
		}

		if (m_globCounter > 30)
		{
			m_centerPosition = m_position;
			m_globCounter = 0.0f;
		}

		//m_position += m_gravityOffset * deltaTime; 
	}
	else if (m_state == "move" || m_state == "update" || m_state == "attack")
	{
		m_lastPos = m_position;
		float speed = m_speed;
		float sqrt_distToTargetPos = sqrt_distance(m_position, m_targetPos);
		
		glm::vec3 move_vector = glm::normalize(m_targetPos - m_position);
		glm::vec3 nextPosition = m_position + move_vector * speed * deltaTime;
		
		float sqrt_distToNextPos = sqrt_distance(m_position, nextPosition);

		if (sqrt_distToNextPos > sqrt_distToTargetPos)
			m_position = m_targetPos;
		else
		{
			//m_position = nextPosition;
						
			if (nextPosition.x > m_game->m_edgesOfMap->left && nextPosition.x < m_game->m_edgesOfMap->right &&
				nextPosition.z > m_game->m_edgesOfMap->bot  && nextPosition.z < m_game->m_edgesOfMap->top)
			{
				m_position = nextPosition;
			}
			else
			{
				m_centerPosition = m_position;
				m_state = "rest";
			}
		}
		
		if (m_position == m_targetPos)
		{
			m_centerPosition = m_position;
			m_state = "rest";
		}
	}

	m_gravityOffset = glm::vec3(0.0f, 0.0f, 0.0f);
}

void Unit::select()
{
	if (!m_selected)
	{
		m_selected = true;

		m_indexes_of_displayd_models.clear();
		m_indexes_of_displayd_models.push_back(0);
		m_indexes_of_displayd_models.push_back(1);
	}
}

void Unit::deselect()
{
	if (m_selected)
	{
		m_selected = false;
		m_indexes_of_displayd_models.clear();
		m_indexes_of_displayd_models.push_back(1);
	}
}