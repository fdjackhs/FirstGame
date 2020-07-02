#include "Object.h"

Object::Object() {}

Object::Object(const std::vector<unsigned int>& ID, const glm::vec3& pos, float scl, const std::string& opt_prop)
{
	m_modelIDs = ID;
	m_position = pos;
	m_scale = scl;
	m_optionalProperties = opt_prop;
	
	m_visible.push_back(true);	//first always model
	m_visible.push_back(false); //second and subsequent - is areas, buffs, effects, etc...
}

Object::~Object()
{

}

void Object::action(float deltaTime)
{

}

glm::vec3 Object::GetPosition() const
{
	return m_position;
}