#pragma once
#include <iostream>
#include <map>

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
	void createUnit(const glm::vec3& position, const glm::vec3& targetPosition);
	void updateGameState(float deltaTime);
	void processInput(bool* keys, bool* buttons, const glm::vec2& cursorCoords, double scroll);
	void selectUnits();
	void updatePhysics();

	//for fast creating units during the game
	std::vector<unsigned int> m_common_unit_index;

private:
	std::vector<std::shared_ptr<Object>> objects;
	//std::vector<Object*> objects;
	std::shared_ptr<select_area> m_area;

	//game state (game, pause)
	//game speed
};