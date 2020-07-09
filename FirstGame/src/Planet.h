#pragma once
#include <glm/glm.hpp>

#include "Object.h"
#include "Game.h"

extern class Game;

class Planet : public Object
{
public:

	float m_reloading;
	Game* m_gameState;

	Planet(const std::vector<unsigned int>& ID, const glm::vec3& pos, float scale, const std::string& opt_prop, Game* game, const std::string fraction);
	void action(float deltaTime);
	glm::vec3 GetPosition() const;
};