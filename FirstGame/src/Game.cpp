#include "Game.h"

void Game::loop()
{
	while (!RenderEngine::windowShouldClose())
	{
		RenderEngine::clearScreen();

		//Game::poolEvents()

		//Game::updateGameState()

		//RenderEngine::drawObjects(Game::all_objects)


		RenderEngine::updateScreen();
		RenderEngine::pollEvents();
	}
}


void Game::load()
{

}

void Game::updateGameState()
{

}

void Game::poolEvents()
{

}