#pragma once
#include "Object.h"
#include "Game.h"

class Game;

class Button : public Object
{
public:
	Button(const std::vector<unsigned int>& ID, const glm::vec3& pos, float scale, Game* game, const std::string& opt_prop);
		
	bool cursorIsOnButton(const glm::vec2& clickCoords);
	void setButtonDown();
	void setButtonUp();

	void setCallbackFunc(void(*func)(Game&));
	void callCallbackFunction();

	bool m_isPressed;
private:
	glm::vec2 m_size;
	glm::vec2 m_left_top_corner;

	Game* m_gameState;
	void(*m_function)(Game&);

	//Update Button vertices according to aspect ratio
	void updateButtonVertices();

	void calculateSizeOfButton();
};