#pragma once
#include <glm/glm.hpp>

#include "Object.h"
#include "Utilities.h"

class Unit : public Object
{
public:
	bool m_annihilated;
	bool m_selected;
	int m_physics_counter;
	float m_speed;

	glm::vec3 m_targetPos;
	glm::vec3 m_gravityOffset;

	std::string m_state;

	Unit(const std::vector<unsigned int>& ID, const glm::vec3& position, float scale, const std::string& opt_prop, const std::string fraction);
	Unit(const std::vector<unsigned int>& ID, const glm::vec3& position, const glm::vec3& target, const std::string fraction);

	void action(float deltaTime);
	glm::vec3 GetPosition() const;
	void setTargetPosition(glm::vec3& target);

	void select();
	void deselect();
};