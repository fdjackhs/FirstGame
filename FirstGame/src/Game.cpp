#include "Game.h"

Game::Game()
{
}

Game::~Game()
{

}

void Game::loop()
{
	double lastTime = RenderEngine::getCurrTime();

	while (!RenderEngine::windowShouldClose())
	{
		double currTime = RenderEngine::getCurrTime();
		RenderEngine::deltaTime = currTime - lastTime;
		lastTime = currTime;

		//std::cout << "Delta Time " << RenderEngine::deltaTime << std::endl;

		processInput(RenderEngine::keys, RenderEngine::buttons, RenderEngine::cursorCoords, RenderEngine::scroll);

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

void Game::processInput(bool* keys, bool* buttons, const glm::vec2& cursorCoords, double scroll)
{
	//process keyboard
	//----------------
	float velocity = RenderEngine::camera->MovementSpeed * (float)RenderEngine::deltaTime;
	/*
	if (keys[87])
		RenderEngine::camera->Position.z -= velocity;
	if (keys[83])
		RenderEngine::camera->Position.z += velocity;
	if (keys[65])
		RenderEngine::camera->Position.x -= velocity;
	if (keys[68])
		RenderEngine::camera->Position.x += velocity;
	*/

	//process mouse scroll
	//--------------------
	if (scroll == 1)
		RenderEngine::camera->Position.y -= 0.5f;
	if (scroll == -1)
		RenderEngine::camera->Position.y += 0.5f;

	if (RenderEngine::camera->Position.y < 3)
		RenderEngine::camera->Position.y = 3;
	if (RenderEngine::camera->Position.y > 40)
		RenderEngine::camera->Position.y = 40;

	RenderEngine::scroll = 0;

	
	//process cursor position
	//-----------------------
	if (!buttons[2])
	{
		if (RenderEngine::enteredWindow)
		{
			float borderx = RenderEngine::SCREEN.x * 0.2f;
			float bordery = RenderEngine::SCREEN.y * 0.4f;

			if (cursorCoords.x < borderx)
				RenderEngine::camera->Position.x -= velocity;
			if (cursorCoords.x > RenderEngine::SCREEN.x - borderx)
				RenderEngine::camera->Position.x += velocity;

			if (cursorCoords.y < bordery)
				RenderEngine::camera->Position.z -= velocity;
			if (cursorCoords.y > RenderEngine::SCREEN.y - bordery)
				RenderEngine::camera->Position.z += velocity;
		}
		RenderEngine::firstMouse = true;
	}
	else
	{
		if (RenderEngine::firstMouse)
		{
			RenderEngine::startCursorPos.x = cursorCoords.x;
			RenderEngine::startCursorPos.z = cursorCoords.y;
			RenderEngine::firstMouse = false;
		}

		glm::vec3 startP = RenderEngine::cursorCoordToWorldCoords({ RenderEngine::startCursorPos.x, RenderEngine::startCursorPos.z });
		glm::vec3 crurrP = RenderEngine::cursorCoordToWorldCoords({ cursorCoords.x , cursorCoords.y });

		RenderEngine::startCursorPos.x = cursorCoords.x;
		RenderEngine::startCursorPos.z = cursorCoords.y;

		RenderEngine::camera->Position += startP - crurrP;
	}
}

