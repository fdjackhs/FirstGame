#pragma once
#include <iostream>

#include "RenderEngine.h"
//#include "Shader.h"
//#include "Camera.h"

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
	void updateGameState(float deltaTime);
	void processInput(bool* keys);

private:
	//std::shared_ptr<Shader> basicShader = nullptr;

	//game state (game, pause)
	//game speed
};