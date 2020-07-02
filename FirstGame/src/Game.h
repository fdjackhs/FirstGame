#pragma once
#include <iostream>
#include <map>

#include "RenderEngine.h"
#include "Unit.h"
#include "SelectArea.h"

class Game
{
public:
	Game();
	~Game();

	Game(Game&) = delete;				//??
	Game(Game&&) = delete;				//??
	Game& operator==(Game&) = delete;	//??

	void loop();
	void load();
	void createObject(const ObjectAttributes& attributes);
	void updateGameState(float deltaTime);
	void processInput(bool* keys, bool* buttons, const glm::vec2& cursorCoords, double scroll);
	void selectUnits();

private:
	std::vector<std::shared_ptr<Object>> objects;
	select_area m_area;

	//game state (game, pause)
	//game speed
};