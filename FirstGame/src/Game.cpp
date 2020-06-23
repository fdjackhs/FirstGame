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

	//Shader basicShader("../FirstGame/Resources/shaders/1.basic_vertex_shader.vs", "../FirstGame/Resources/shaders/1.basic_fragment_shader.fs");
	//Model guitar("../FirstGame/Resources/models/backpack/backpack.obj");


	while (!RenderEngine::windowShouldClose())
	{
		double currTime = RenderEngine::getCurrTime();
		RenderEngine::deltaTime = currTime - lastTime;
		lastTime = currTime;

		//std::cout << "Delta Time " << RenderEngine::deltaTime << std::endl;

		processInput(RenderEngine::keys);

		RenderEngine::clearScreen();

		RenderEngine::updateCameraView();

		//guitar.Draw(basicShader);

		//Game::updateGameState(deltaTime)

		RenderEngine::drawObjects(Game::objects);

		RenderEngine::updateScreen();
		RenderEngine::pollEvents();
	}
}


void Game::load()
{
	std::vector<ObjectAttributes> objectsAttribs;
	RenderEngine::resourceManager.loadLevel(0, objectsAttribs);

	for (auto&& x : objectsAttribs)
	{
		createObject(x);
	}
}

void Game::createObject(ObjectAttributes attributes)
{

}

void Game::updateGameState(float deltaTime)
{
	//TODO: update game logic

}

void Game::processInput(bool* keys)
{
	//TODO: process release keys

	if (keys[87])
		RenderEngine::camera->ProcessKeyboard(Camera::FORWARD, (float)RenderEngine::deltaTime);
	if (keys[83])
		RenderEngine::camera->ProcessKeyboard(Camera::BACKWARD, (float)RenderEngine::deltaTime);
	if (keys[65])
		RenderEngine::camera->ProcessKeyboard(Camera::LEFT, (float)RenderEngine::deltaTime);
	if (keys[68])
		RenderEngine::camera->ProcessKeyboard(Camera::RIGHT, (float)RenderEngine::deltaTime);

	RenderEngine::clearKeys();
}