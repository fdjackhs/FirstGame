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

	bool m_annihilated;
	bool m_selected;
	float m_scale;
	float m_healthPoints;

	float m_reloadingTime;
	float m_level;

	float m_radius;

	int m_physics_counter;

	std::string m_type;

	std::string m_state;

	glm::vec3 m_position;
	glm::vec3 m_targetPos;
	glm::vec3 m_gravityOffset;

	std::string m_fraction; //color

	std::vector<unsigned int> m_modelIDs;
	//Indices of all models to be displayed
	std::vector<unsigned int> m_indexes_of_displayd_models;

	std::string m_optionalProperties;

	virtual void action(float);
	virtual void setTargetPosition(glm::vec3& target);
	virtual glm::vec3 GetPosition() const;
};