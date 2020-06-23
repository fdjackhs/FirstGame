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

	std::map<std::string, std::vector<Object>> objects;

	void loop();
	void load();
	void createObject(ObjectAttributes attributes);
	void updateGameState(float deltaTime);
	void processInput(bool* keys);

private:
	//game state (game, pause)
	//game speed
};