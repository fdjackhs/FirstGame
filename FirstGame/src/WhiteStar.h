#pragma once
#include "Object.h"

class WhiteStar : public Object
{
public:
	WhiteStar(const std::vector<unsigned int>& ID, const glm::vec3& pos, float scale, const std::string& opt_prop, const std::string fraction)
	{
		m_modelIDs = ID;
		m_indexes_of_displayd_models.push_back(0);

		m_position = pos;
		m_scale = scale;
		m_optionalProperties = opt_prop;
		m_type = STAR;
		m_fraction = fraction;
		m_visible = true;
	}
	~WhiteStar() {}
};