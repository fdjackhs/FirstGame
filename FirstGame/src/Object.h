#pragma once
#include <string>
#include <vector>

#include "glm/vec2.hpp"
#include "glm/vec3.hpp"


class Object
{
public:
	Object();
	Object(const std::vector<unsigned int>& ID, const glm::vec3& pos, float scale, const std::string& opt_prop);
	~Object();

	bool m_selected;
	float m_scale;
	glm::vec3 m_position;
	glm::vec3 m_targetPos;
	std::string m_optionalProperties;

	std::vector<unsigned int> m_modelIDs;

	virtual void action(float);
	virtual void setTargetPosition(glm::vec3& target);
	virtual glm::vec3 GetPosition() const;
};