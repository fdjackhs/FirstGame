#pragma once
#include <glm/glm.hpp>

#include "Object.h"

class Unit : public Object
{
public:

	float m_speed;

	Unit(const std::vector<unsigned int>& ID, const glm::vec3& position, float scale, const std::string& opt_prop, const std::string fraction);
	Unit(const std::vector<unsigned int>& ID, const glm::vec3& position, const glm::vec3& target, const std::string fraction);

	void action(float deltaTime);
	glm::vec3 GetPosition() const;
};