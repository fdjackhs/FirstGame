#include "Object.h"

Object::Object() {}

Object::Object(const std::vector<unsigned int>& ID, const glm::vec3& pos, float scl, const std::string& opt_prop, const std::string fraction)
{
	m_modelIDs = ID;
	m_indexes_of_displayd_models.push_back(0);

	m_position = pos;
	m_targetPos = m_position;
	m_scale = scl;
	m_optionalProperties = opt_prop;
	m_selected = false;
	m_type = "Object";
	m_physics_counter = 0;

	m_fraction = fraction; //color
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