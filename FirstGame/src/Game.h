#pragma once
#include <iostream>
#include <map>
#include <memory>

#include "RenderEngine.h"
#include "Unit.h"
#include "Planet.h"
#include "SelectArea.h"

class Game
{
public:
	Game(unsigned int level);
	~Game();

	Game(Game&) = delete;				//??
	Game(Game&&) = delete;				//??
	Game& operator==(Game&) = delete;	//??

	void loop();
	void createObject(const ObjectAttributes& attributes);
	void createUnit(const std::string& unitType, const glm::vec3& position, const glm::vec3& targetPosition, const std::string& fraction);
	void updateGameState(float deltaTime);
	void processInput(bool* keys, bool* buttons, const glm::vec2& cursorCoords, double scroll);
	void selectUnits();
	void updatePhysics();
	void collisionsDetected();

	inline bool closerThan(const glm::vec3& firstPosition, const glm::vec3& secondPosition, const float& distance); //custom distance

	//for fast creating units during the game
	//std::vector<unsigned int> m_common_unit_index;

private:
	std::vector<std::shared_ptr<Object>> objects;

	std::shared_ptr<select_area> m_area;

	std::string m_player_fraction;
	std::string m_ai_fraction;

	//game state (game, pause)
	//game speed
};