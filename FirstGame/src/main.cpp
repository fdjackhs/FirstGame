#include <iostream>

#include "RenderEngine.h"
#include "Game.h"

#include <glfw/glfw3.h>

int main()
{	
	RenderEngine::init(1600, 800, "FirstGame");

	Game game;
	game.load();
	//game init
	game.loop();

	RenderEngine::free();
	return 0;
}
