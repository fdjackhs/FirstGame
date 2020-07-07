#include "Game.h"

Game::Game(unsigned int level)
{
	std::vector<ObjectAttributes> objectsAttribs;
	RenderEngine::resourceManager.loadLevel(0, objectsAttribs);

	for (auto&& x : objectsAttribs)
	{
		createObject(x);
	}

	//create objects manually

	uint32_t id = RenderEngine::resourceManager.createObject(std::vector<float>(240), "../FirstGame/Resources/shaders/1.area_shader.vs", "../FirstGame/Resources/shaders/1.area_shader.fs");
	m_area = std::make_shared<select_area>(id);
}

Game::~Game()
{

}

void Game::loop()
{
	float lastTime = RenderEngine::getCurrTime();

	while (!RenderEngine::windowShouldClose())
	{
		float currTime = RenderEngine::getCurrTime();
		RenderEngine::deltaTime = currTime - lastTime;
		lastTime = currTime;

		//std::cout << "Delta Time " << RenderEngine::deltaTime << std::endl;

		processInput(RenderEngine::keys, RenderEngine::buttons, RenderEngine::cursorCoords, RenderEngine::scroll);

		RenderEngine::clearScreen();
		
		RenderEngine::updateCameraView();

		updateGameState(RenderEngine::deltaTime);

		RenderEngine::drawObjects(Game::objects/*, Game::m_area*/);

		RenderEngine::updateScreen();
		RenderEngine::pollEvents();
	}
}


void Game::createObject(const ObjectAttributes& attributes)
{
	if (attributes.optionalProperties == "UNIT")
	{
		Unit* unit = new Unit(attributes.IDs,
							  glm::vec3{ stof(attributes.posx),
										 stof(attributes.posy),
										 stof(attributes.posz) },
							  stof(attributes.scale),
							  "");
		std::shared_ptr<Object> obj(unit);
		objects.push_back(obj);
	}
	else
	{
		Object* unit = new Object(attributes.IDs,
								  glm::vec3{ stof(attributes.posx),
											 stof(attributes.posy),
											 stof(attributes.posz) },
								  stof(attributes.scale),
								  "");
		std::shared_ptr<Object> obj(unit);
		objects.push_back(obj);
	}
}

void Game::updatePhysics()
{
	for (uint32_t i = 0; i < objects.size(); i++)
	{
		glm::vec3 totoalGravityVector = { 0.0f, 0.0f, 0.0f };

		for (uint32_t j = 0; j < objects.size(); j++)
		{
			if (j != i)
			{
				float force = 1.0f / sqrt(glm::distance(objects[i]->m_position, objects[j]->m_position)) * 0.01f;
				if (force < 0.02)
					force = 0.0f;
				glm::vec3 temp = glm::normalize(objects[i]->m_position - objects[j]->m_position) * force;

				totoalGravityVector += temp;
			}
		}

		objects[i]->m_gravityOffset = totoalGravityVector;
	}
}

void Game::updateGameState(float deltaTime)
{
	updatePhysics();

	selectUnits();

	for (auto&& obj : objects)
	{
		obj->action(deltaTime);
	}
}

void Game::selectUnits()
{
	for (auto&& obj : objects)
	{
		if (glm::distance(m_area->m_position, obj->m_position) <= m_area->radius)
		{
			obj->m_selected = true;
		}
		else
		{
			obj->m_selected = false;
		}
	}
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
		/*
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
		*/
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


	static glm::vec2 lastCursorCoords;

	//selected area
	if (buttons[0])
	{
		if (cursorCoords != lastCursorCoords)
		{
			if (m_area->firstTime)
			{
				m_area->areaStartPosition = cursorCoords;
				m_area->firstTime = false;
			}

			float radius = glm::distance(m_area->areaStartPosition, cursorCoords);
			glm::vec2 diff = glm::vec2(cursorCoords.x - m_area->areaStartPosition.x, cursorCoords.y - m_area->areaStartPosition.y);
			glm::vec2 center = m_area->areaStartPosition + glm::vec2(diff.x / 2, diff.y / 2);

			glm::vec3 centerInWorldCoords = RenderEngine::cursorCoordToWorldCoords({ center.x, center.y });
			glm::vec3 borderInWorldCoords = RenderEngine::cursorCoordToWorldCoords({ cursorCoords.x, cursorCoords.y });

			uint32_t indexOfarea = RenderEngine::resourceManager.m_manCrObj_indexs[m_area->ID].first;

			RenderEngine::resourceManager.m_manuallyCreaatedObjects[indexOfarea].m_position = centerInWorldCoords;
			m_area->m_position = centerInWorldCoords;
			m_area->radius = glm::distance(centerInWorldCoords, borderInWorldCoords);

			m_area->updateVertices();
			RenderEngine::resourceManager.updateVBO(m_area->ID, m_area->vertices);
		}

		m_area->previusTime = true;
	}
	else
	{
		//set target
		if (m_area->previusTime && m_area->firstTime)
		{
			glm::vec3 targetPos = RenderEngine::cursorCoordToWorldCoords({ cursorCoords.x, cursorCoords.y });
			for (auto&& obj : objects)
			{
				if (obj->m_selected)
				{
					obj->setTargetPosition(targetPos);
					obj->m_state = "move";
				}
			}
			
			//collapse select area
			m_area->radius = 0.0f;
			m_area->updateVertices();
			RenderEngine::resourceManager.updateVBO(m_area->ID, m_area->vertices);
		}

		m_area->firstTime = true;
		m_area->previusTime = false;
	}

	lastCursorCoords = cursorCoords;
}

