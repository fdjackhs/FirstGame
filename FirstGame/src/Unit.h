#pragma once
#include <glm/glm.hpp>

#include "Object.h"

class Unit : public Object
{
public:
	Unit(const std::vector<unsigned int>& ID, const glm::vec3& pos, float scale, const std::string& opt_prop);

	std::string m_state;

	float m_speed;
	float m_front;

	glm::vec3 m_target;
	glm::vec3 m_actualPosition;

	void action(float deltaTime);
	glm::vec3 GetPosition() const;
};