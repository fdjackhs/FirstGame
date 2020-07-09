#pragma once
#include <string>
#include <vector>

#include "glm/vec2.hpp"
#include "glm/vec3.hpp"


class Object
{
public:
	Object();
	Object(const std::vector<unsigned int>& ID, const glm::vec3& pos, float scale, const std::string& opt_prop, const std::string fraction);
	~Object();

	bool m_selected;
	float m_scale;

	int m_physics_counter;

	std::string m_type;

	std::string m_state;

	glm::vec3 m_position;
	glm::vec3 m_targetPos;

	glm::vec3 m_gravityOffset;

	std::string m_optionalProperties;

	std::string m_fraction; //color

	std::vector<unsigned int> m_modelIDs;

	virtual void action(float);
	virtual void setTargetPosition(glm::vec3& target);
	virtual glm::vec3 GetPosition() const;
};