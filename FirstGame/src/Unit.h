#pragma once
#include <glm/glm.hpp>

#include "Object.h"

class Unit : public Object
{
public:

	float m_speed;

	Unit(const std::vector<unsigned int>& ID, const glm::vec3& position, float scale, const std::string& opt_prop);
	Unit(const std::vector<unsigned int>& ID, const glm::vec3& position, const glm::vec3& target);

	void action(float deltaTime);
	glm::vec3 GetPosition() const;
};