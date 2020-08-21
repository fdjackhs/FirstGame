#pragma once
#include <glm/glm.hpp>

#include "Object.h"
#include "Game.h"

extern class Game;

class Planet : public Object
{
public:
	bool m_selected;

	float m_capturePoints;
	float m_upgradePoints;

	float m_reloadingTime;
	float m_level;

	float m_reloading;
	Game* m_gameState;

	// index of capture and upgrade scales in resource manager in rm->m_models
	uint32_t m_Real_IndexOfCaputreScaleRM;
	uint32_t m_Real_IndexOfUpgradeScaleRM;

	const static int s_max_level = 3;

	Planet(const std::vector<unsigned int>& ID, const glm::vec3& pos, float scale, const std::string& opt_prop, Game* game, const std::string fraction);

	glm::vec3 GetPosition() const;

	void action(float deltaTime);
	void plusOne();
	void minusOne(const std::string& unitFraction);
	void determineColor();

	void updateScaleVertices();

	void select();
	void deselect();
};