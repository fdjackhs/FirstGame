#pragma once
#include <string>
#include <vector>

#include "glm/vec2.hpp"
#include "glm/vec3.hpp"

class Object
{
public:
	enum ObjectType { OBJECT, UNIT, PLANET, BUTTON };

	Object();
	Object(const std::vector<unsigned int>& ID, const glm::vec3& pos, float scale, const std::string& opt_prop, const std::string fraction);
	~Object();
	
	float m_radius;
	float m_scale;
	ObjectType m_type;

	glm::vec3 m_position;

	std::vector<unsigned int> m_modelIDs;
	//Indices of all models to be displayed
	std::vector<unsigned int> m_indexes_of_displayd_models;

	std::string m_fraction; //color
	std::string m_optionalProperties;

	//-------------------------------
	virtual void action(float);
	virtual glm::vec3 GetPosition() const;
};