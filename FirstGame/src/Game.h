#pragma once

#include "RenderEngine.h"

class Game
{
public:
	Game();
	~Game();

	Game(Game&) = delete;				//??
	Game(Game&&) = delete;				//??
	Game& operator==(Game&) = delete;	//??

	std::vector<Object> objects;

	void loop();
	void load();
	void updateGameState();
	void poolEvents();

private:

	//game state (game, pause)
	//game speed
};