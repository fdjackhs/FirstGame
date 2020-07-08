#include "Object.h"

Object::Object() {}

Object::Object(const std::vector<unsigned int>& ID, const glm::vec3& pos, float scl, const std::string& opt_prop)
{
	m_modelIDs = ID;
	m_position = pos;
	m_targetPos = m_position;
	m_scale = scl;
	m_optionalProperties = opt_prop;
	m_selected = false;
	m_type = "Object";
}

Object::~Object()
{

}

void Object::action(float deltaTime)
{
}

void Object::setTargetPosition(glm::vec3& target)
{
	m_targetPos = target;
}

glm::vec3 Object::GetPosition() const
{
	return m_position;
}