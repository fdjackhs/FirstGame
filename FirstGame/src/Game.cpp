#include "Game.h"

Game::Game()
{
}

Game::~Game()
{

}

void Game::loop()
{
	double lastTime = 0.0;

	while (!RenderEngine::windowShouldClose())
	{
		double currTime = RenderEngine::getCurrTime();
		RenderEngine::deltaTime = currTime - lastTime;
		lastTime = currTime;

		//std::cout << "Delta Time " << RenderEngine::deltaTime << std::endl;

		processInput(RenderEngine::keys);

		RenderEngine::clearScreen();
		
		RenderEngine::updateCameraView();

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

void Game::createObject(const ObjectAttributes& attributes)
{
	Object obj(attributes.ID,
			   glm::vec3{ stof(attributes.posx),
						  stof(attributes.posy),
						  stof(attributes.posz) },
			   stof(attributes.scale),
			   "");

	objects.push_back(obj);
}

void Game::updateGameState(float deltaTime)
{
	//TODO: update game logic

}

void Game::processInput(bool* keys)
{
	if (keys[87])
		RenderEngine::camera->ProcessKeyboard(Camera::FORWARD, (float)RenderEngine::deltaTime);
	if (keys[83])
		RenderEngine::camera->ProcessKeyboard(Camera::BACKWARD, (float)RenderEngine::deltaTime);
	if (keys[65])
		RenderEngine::camera->ProcessKeyboard(Camera::LEFT, (float)RenderEngine::deltaTime);
	if (keys[68])
		RenderEngine::camera->ProcessKeyboard(Camera::RIGHT, (float)RenderEngine::deltaTime);
}