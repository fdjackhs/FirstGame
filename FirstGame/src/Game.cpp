#include <chrono>
#include <iostream>
#include <thread>

#include "Game.h"

Game::Game()
{

}

Game::~Game()
{

}

void Game::loop()
{
	auto lastTime = std::chrono::high_resolution_clock::now();

	while (!RenderEngine::windowShouldClose())
	{
		auto currTime = std::chrono::high_resolution_clock::now();
		auto deltaTime = currTime - lastTime;
		auto lastTime = currTime;

		//std::cout << "Delta Time " << std::chrono::duration_cast<std::chrono::nanoseconds>(currTime - lastTime).count() << std::endl;

		RenderEngine::clearScreen();

		//Game::poolEvents()

		//Game::updateGameState(deltaTime)

		//RenderEngine::drawObjects(Game::all_objects)


		RenderEngine::updateScreen();
		RenderEngine::pollEvents();
	}
}


void Game::load()
{
	//TODO: load resources


}

void Game::updateGameState()
{
	//TODO: update game logic
}

void Game::poolEvents()
{
	//TODO: pool events

	//for (auto&& x : RenderEngine::keys)
	//	x = false;
}