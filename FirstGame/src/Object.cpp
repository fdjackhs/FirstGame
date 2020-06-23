#include "Object.h"

Object::Object(unsigned int ID, const glm::vec3& pos, float scl, const std::string& opt_prop)
{
	modelID = ID;
	position = pos;
	scale = scl;
	optionalProperties = opt_prop;
}

Object::~Object()
{

}
