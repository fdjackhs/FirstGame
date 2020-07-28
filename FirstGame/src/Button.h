#pragma once
#include "Object.h"
#include "Game.h"

class Button : public Object
{
public:
	Button(const std::vector<unsigned int>& ID, const glm::vec3& pos, float scale, const std::string& opt_prop);

	//setFunciton
	//setTextures

	bool checkButtonPress(const glm::vec2& clickCoords);
	void setButtonDown();
	void setButtonUp();

	bool m_isPressed;
private:
	glm::vec2 m_size;
	glm::vec2 m_left_top_corner;
};