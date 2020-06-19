#include "Game.h"

void Game::loop()
{
	while (!RenderEngine::windowShouldClose())
	{
		RenderEngine::clearScreen();
		RenderEngine::updateScreen();
		RenderEngine::pollEvents();
	}
}