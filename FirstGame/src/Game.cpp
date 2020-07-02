#include "Game.h"

Game::Game()
{
	m_area.areaStartPosition = glm::vec3(0.0f, 0.0f, 0.0f);
	m_area.firstTime = true;

	for (int i = 0; i < 240; i++) m_area.vertices[i] = 0.0f;
	m_area.vbo = RenderEngine::resourceManager.createObject(m_area.vertices, "../FirstGame/Resources/shaders/1.area_shader.vs", "../FirstGame/Resources/shaders/1.area_shader.fs", m_area.vao);
}

Game::~Game()
{

}

void Game::loop()
{

	//m_area.vbo = RenderEngine::resourceManager.createObject(m_area.vertices, "../FirstGame/Resources/shaders/1.area_shader.vs", "../FirstGame/Resources/shaders/1.area_shader.fs", m_area.vao);

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

		RenderEngine::drawObjects(Game::objects, Game::m_area);

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

void Game::updateGameState(float deltaTime)
{
	/*
	static float frame = 0;
	frame += deltaTime;
	*/
	for (auto&& obj : objects)
	{
		/*
		if (frame >= 1.0f)
		{
			obj->m_visible[1] = !obj->m_visible[1];
			frame = 0;
		}*/

		obj->action(deltaTime);
	}
}

void Game::selectUnits()
{
	for (auto&& obj : objects)
	{
		if (glm::distance(m_area.centerInWorldCoords, obj->m_position) <= m_area.radiusInWorldCoords)
		{
			obj->m_selected = true;
			//obj->m_scale = 0.2f;
		}
		else
		{
			obj->m_selected = false;
			//obj->m_scale = 0.1f;
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

	//selected area
	if (buttons[0])
	{
		if (m_area.firstTime)
		{
			m_area.areaStartPosition = cursorCoords;
			m_area.firstTime = false;
		}

		m_area.radius = glm::distance(m_area.areaStartPosition, cursorCoords);
		glm::vec2 diff = glm::vec2(cursorCoords.x - m_area.areaStartPosition.x, cursorCoords.y - m_area.areaStartPosition.y);
		m_area.center = m_area.areaStartPosition + glm::vec2(diff.x / 2, diff.y / 2);

		m_area.centerInWorldCoords = RenderEngine::cursorCoordToWorldCoords({ m_area.center.x, m_area.center.y });
		glm::vec3 borderInWorldCoords = RenderEngine::cursorCoordToWorldCoords({ cursorCoords.x, cursorCoords.y });
		m_area.radiusInWorldCoords = glm::distance(m_area.centerInWorldCoords, borderInWorldCoords);

		selectUnits();
		//objects[0]->m_position = RenderEngine::cursorCoordToWorldCoords({ m_area.center.x, m_area.center.y });
	}
	else
	{
		m_area.firstTime = true;
	}
}

