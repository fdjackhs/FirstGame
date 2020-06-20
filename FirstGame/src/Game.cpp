#include <chrono>
#include <iostream>
#include <thread>

#include "Game.h"

Game::Game()
{
	basicShader = std::make_shared<Shader>("../FirstGame/Resources/shaders/1.basic_vertex_shader.vs", "../FirstGame/Resources/shaders/1.basic_fragment_shader.fs");
}

Game::~Game()
{

}

void Game::loop()
{
	auto lastTime = std::chrono::high_resolution_clock::now();

	RenderEngine::initTriangle();


	while (!RenderEngine::windowShouldClose())
	{
		auto currTime = std::chrono::high_resolution_clock::now();
		auto deltaTime = currTime - lastTime;
		auto lastTime = currTime;

		//std::cout << "Delta Time " << std::chrono::duration_cast<std::chrono::nanoseconds>(currTime - lastTime).count() << std::endl;

		RenderEngine::clearScreen();

		basicShader->use();
		RenderEngine::drawTriangle();
		
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