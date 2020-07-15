#pragma once
#include <glm/glm.hpp>

#include "Object.h"

class Unit : public Object
{
public:
	bool m_annihilated;
	int m_physics_counter;
	glm::vec3 m_targetPos;
	std::string m_state;
	glm::vec3 m_gravityOffset;

	float m_speed;

	Unit(const std::vector<unsigned int>& ID, const glm::vec3& position, float scale, const std::string& opt_prop, const std::string fraction);
	Unit(const std::vector<unsigned int>& ID, const glm::vec3& position, const glm::vec3& target, const std::string fraction);

	void action(float deltaTime);
	glm::vec3 GetPosition() const;
	void setTargetPosition(glm::vec3& target);
};