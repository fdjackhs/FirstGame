#pragma once
#include <vector>

#include "glm/glm.hpp"

#include "Shader.h"
#include "Utilities.h"

class Label
{
public:
	bool m_visible;
	float m_scale;
	float m_widthOfChar;

	std::string m_label;
	std::string m_optionalProperties;
	glm::vec3 m_position;

	Var m_corresponding_value;
	std::vector<uint32_t> m_value;

	std::vector<uint32_t> m_alphabet;

	Label();
	Label(const std::vector<uint32_t>& alphabet, const Var& value, glm::vec3& position, const float& scale, const std::string& optProp);
	Label(const std::vector<uint32_t>& alphabet, const std::string& label, glm::vec3& position, const float& scale, const std::string& optProp);


	void updateLabel();
	void updateLabelVertices();
};