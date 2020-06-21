
#include "Game.h"

Game::Game()
{
	//basicShader = std::make_shared<Shader>("../FirstGame/Resources/shaders/1.basic_vertex_shader.vs", "../FirstGame/Resources/shaders/1.basic_fragment_shader.fs");
}

Game::~Game()
{

}

void Game::loop()
{
	double lastTime = 0.0;

	//Model guitar("../FirstGame/Resources/models/backpack/backpack.obj");

	while (!RenderEngine::windowShouldClose())
	{
		double currTime = RenderEngine::getCurrTime();
		RenderEngine::deltaTime = currTime - lastTime;
		lastTime = currTime;

		//std::cout << "Delta Time " << RenderEngine::deltaTime << std::endl;

		processInput(RenderEngine::keys);


		RenderEngine::clearScreen();
		//RenderEngine::updateCameraView(deltaTime);
		
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

void Game::updateGameState(float deltaTime)
{
	//TODO: update game logic

}

void Game::processInput(bool* keys)
{
	//TODO: process release keys

	//RenderEngine::camera->ProcessKeyboard(Camera::FORWARD, deltaTime);

	RenderEngine::clearKeys();
}