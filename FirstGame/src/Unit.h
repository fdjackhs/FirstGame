#pragma once
#include <glm/glm.hpp>

#include "Object.h"

class Game;

class Unit : public Object
{
public:
	bool m_annihilated;
	bool m_selected;
	float m_physCounter;
	float m_globCounter;
	float m_speed;

	float m_lastAngle;

	glm::vec3 m_centerPosition;
	glm::vec3 m_currVector;

	glm::vec3 m_targetPos;
	glm::vec3 m_gravityOffset;
	glm::vec3 m_lastPos;

	std::string m_state;
	Game* m_game;

	Unit(const std::vector<unsigned int>& ID, const glm::vec3& position, float scale, const std::string& opt_prop, const std::string fraction, Game* game);

	void action(float deltaTime);
	void select();
	void deselect();
};