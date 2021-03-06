#include "Object.h"
#include <iostream>


Object::Object() 
{
	m_visible = true;
}

Object::Object(const std::vector<unsigned int>& ID, const glm::vec3& pos, float scl, const std::string& opt_prop, const std::string fraction)
{
	m_modelIDs = ID;
	m_indexes_of_displayd_models.push_back(0);

	m_position = pos;
	m_scale = scl;
	m_optionalProperties = opt_prop;

	m_type = OBJECT;

	m_fraction = fraction; //color

	m_visible = true;
}

Object::~Object()
{

}