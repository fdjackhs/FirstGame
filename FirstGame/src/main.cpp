#include "RenderEngine.h"
#include "Game.h"

#include <iostream>

Game game;

int main()
{
	RenderEngine::init(1600, 900, "FirstGame");

	game.loop();

	RenderEngine::free();
	return 0;
}
