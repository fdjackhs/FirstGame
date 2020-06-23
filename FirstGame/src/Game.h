#pragma once
#include <iostream>
#include <map>

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
	void createObject(const ObjectAttributes& attributes);
	void updateGameState(float deltaTime);
	void processInput(bool* keys);

private:
	//game state (game, pause)
	//game speed
};