#include "Game.h"
#include "Profile.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glad/glad.h>

#include <GLFW/glfw3native.h>

static bool flag = false;
static uint32_t level = 1;

void Game::loadResources()
{
	Model loadScreen("../FirstGame/resources/models/Loading/LoadScreen.obj");
	Shader interfaceShader("../FirstGame/Resources/shaders/1.button_shader.vs", "../FirstGame/Resources/shaders/1.button_shader.fs");

	loadScreen.Draw(interfaceShader);
	RenderEngine::updateScreen();

	uint32_t progress = 0;
	createGame(level, progress);
}

Game::Game(uint32_t level)
{
	level = 1;
	loadResources();
}
Game::~Game() {}

void startButton(Game& game)
{
	game.m_objects.clear();
	RenderEngine::resourceManager.clear();
	RenderEngine::modelsGroups.clear();

	level = 0;
	game.loadResources();
}

void Game::createGame(uint32_t& level, uint32_t& progress)
{
	m_player_fraction = "RED";
	m_ai_fraction = "BLUE";
	m_area = nullptr;

	std::vector<ObjectAttributes> objectsAttribs;
	RenderEngine::loadLevel(level, objectsAttribs, progress);

	for (auto&& x : objectsAttribs)
		createObject(x);

	m_pause = false;

	lastTime = RenderEngine::getCurrTime();
	RenderEngine::deltaTime = 0.0f;
}

static int counter = 0;

void Game::loop()
{
	lastTime = RenderEngine::getCurrTime();

	while (!RenderEngine::windowShouldClose())
	{
		//LOG_DURATION("--- main loop");

		{
			//LOG_DURATION("calculate deltaTime");

			if (!m_pause)
			{
				float currTime = RenderEngine::getCurrTime();
				RenderEngine::deltaTime = currTime - lastTime;
				lastTime = currTime;

				RenderEngine::deltaTime *= RenderEngine::m_timeKoef;
			}
			else
			{
				lastTime = RenderEngine::getCurrTime();
				RenderEngine::deltaTime = 0.0f;
			}
		}


		{
			//LOG_DURATION("process input");
			processInput(RenderEngine::keys, RenderEngine::buttons, RenderEngine::cursorCoords, RenderEngine::scroll);
		}

		{
			//LOG_DURATION("clearScreen");
			RenderEngine::clearScreen();
		}

		{
			//LOG_DURATION("updateCameraView");
			RenderEngine::updateCameraView();
		}

		{
			LOG_DURATION("updateGameState");
			updateGameState(RenderEngine::deltaTime);
		}

		{
			LOG_DURATION("genModelMatrices");
			RenderEngine::genModelMatrices(Game::m_objects, Game::m_red_units, Game::m_blue_units);
		}

		{
			LOG_DURATION("drawObjects");
			RenderEngine::drawObjects(m_labels);
		}

		{
			LOG_DURATION("updateScreen");
			RenderEngine::updateScreen();
		}

		{
			//LOG_DURATION("pollEvents");
			RenderEngine::pollEvents();
		}


		std::cerr << "objects " << m_objects.size() + m_red_units.size() + m_blue_units.size() << std::endl;
		
		{
			//LOG_DURATION("Profile");
			if (RenderEngine::deltaTime > (1.0f / 59.0f) * RenderEngine::m_timeKoef)
			{
				counter++;
			}
			else
			{
				counter = 0;
			}

			if (counter > 50)
			{
				std::cerr << std::endl;
				std::cerr << "total objects " << m_objects.size() + m_red_units.size() + m_blue_units.size() << std::endl;
				for (auto&& group : RenderEngine::modelsGroups)
				{
					std::cerr << "buffer " << group.buffer << " model_index " << group.model_index << " matrices.size " << group.matrices.size() << std::endl;
				}

				std::cerr << std::endl;
				std::cout << m_objects.size() << std::endl;
				system("pause");
			}
		}
	}
}

void Game::createObject(const ObjectAttributes& attributes)
{
	if (attributes.exsist == "true")
	{
		if (attributes.object_type == "RED_UNIT")
		{
			for (int i = 0; i < 50; i++)
			{
				//Unit* ptr_unit = new Unit(RenderEngine::resourceManager.m_complete_models[attributes.object_type],
				Unit* ptr_unit = new Unit(RenderEngine::resourceManager.m_complete_models[attributes.id],
					glm::vec3{ stof(attributes.posx) - 50.0f,
							   stof(attributes.posy),
							   stof(attributes.posz) + float(i) },
					stof(attributes.scale),
					"", "RED");
				std::shared_ptr<Object> sp_unit(ptr_unit);
				//m_objects.push_back(sp_unit);
				m_red_units.push_back(sp_unit);
			}
		}
		else if (attributes.object_type == "BLUE_UNIT")
		{
			for (int i = 0; i < 0; i++)
			{
				//Unit* ptr_unit = new Unit(RenderEngine::resourceManager.m_complete_models[attributes.object_type],
				Unit* ptr_unit = new Unit(RenderEngine::resourceManager.m_complete_models[attributes.id],
					glm::vec3{ stof(attributes.posx) + 50.0f,
							   stof(attributes.posy),
							   stof(attributes.posz) + float(i) },
					stof(attributes.scale),
					"", "BLUE");
				std::shared_ptr<Object> sp_unit(ptr_unit);
				//m_objects.push_back(sp_unit);
				m_blue_units.push_back(sp_unit);
			}
		}
		else if (attributes.object_type == "RED_PLANET")
		{
			//Planet* ptr_planet = new Planet(RenderEngine::resourceManager.m_complete_models[attributes.object_type],
			Planet* ptr_planet = new Planet(RenderEngine::resourceManager.m_complete_models[attributes.id],
											glm::vec3{ stof(attributes.posx),
													   stof(attributes.posy),
													   stof(attributes.posz) },
											stof(attributes.scale),
											"",
											this, "RED");
			std::shared_ptr<Object> sh_planet(ptr_planet);
			m_objects.push_back(sh_planet);
		}
		else if (attributes.object_type == "BLUE_PLANET")
		{
			//Planet* ptr_planet = new Planet(RenderEngine::resourceManager.m_complete_models[attributes.object_type],
			Planet* ptr_planet = new Planet(RenderEngine::resourceManager.m_complete_models[attributes.id],
											glm::vec3{ stof(attributes.posx),
													   stof(attributes.posy),
													   stof(attributes.posz) },
											stof(attributes.scale),
											"",
											this, "BLUE");
			std::shared_ptr<Object> sh_planet(ptr_planet);
			m_objects.push_back(sh_planet);
		}
		else if (attributes.object_type == "NEUTRAL_PLANET")
		{
			//Planet* ptr_planet = new Planet(RenderEngine::resourceManager.m_complete_models[attributes.object_type],
			Planet* ptr_planet = new Planet(RenderEngine::resourceManager.m_complete_models[attributes.id],
											glm::vec3{ stof(attributes.posx),
													   stof(attributes.posy),
													   stof(attributes.posz) },
											stof(attributes.scale),
											"",
											this, "NEUTRAL");
			std::shared_ptr<Object> sh_planet(ptr_planet);
			m_objects.push_back(sh_planet);
		}
		else if (attributes.object_type == "BUTTON")
		{
			//Button* ptr_button = new Button(RenderEngine::resourceManager.m_complete_models[attributes.object_type],
			Button* ptr_button = new Button(RenderEngine::resourceManager.m_complete_models[attributes.id],
										    glm::vec3{ stof(attributes.posx),
												       stof(attributes.posy),
												       stof(attributes.posz) },
										    stof(attributes.scale),
											this,
										    "");
			std::shared_ptr<Button> sh_button(ptr_button);

			if (attributes.optionalProperties == "PAUSE")
				sh_button->setCallbackFunc(switchPause);
			if (attributes.optionalProperties == "SPEED_UP")
				sh_button->setCallbackFunc(speedGameUp);
			if (attributes.optionalProperties == "SPEED_DOWN")
				sh_button->setCallbackFunc(speedGameDown);
			if (attributes.optionalProperties == "BUTTON_START")
				sh_button->setCallbackFunc(startButton);

			m_objects.push_back(sh_button);
		}
		else if (attributes.object_type == "CAMERA")
		{
			RenderEngine::camera = std::make_shared<Camera>(glm::vec3(stof(attributes.posx), stof(attributes.posy), stof(attributes.posz)));
		}
		else if (attributes.object_type == "AREA")
		{
			//create objects manually
			uint32_t id = RenderEngine::resourceManager.createObject(std::vector<float>(240), "../FirstGame/Resources/shaders/1.area_shader.vs", "../FirstGame/Resources/shaders/1.area_shader.fs");
			m_area = std::make_shared<select_area>(id);
		}
		else if (attributes.object_type == "LABEL")
		{
			//Label* label = new Label(RenderEngine::resourceManager.m_complete_models[attributes.object_type],
			Label* label = new Label(RenderEngine::resourceManager.m_complete_models[attributes.id],
									 Var("float", &RenderEngine::m_timeKoef), 
									 glm::vec3(stof(attributes.posx), stof(attributes.posy), stof(attributes.posz)), 
									 stof(attributes.scale));
			std::shared_ptr<Label> sh_obj(label);
			m_labels.push_back(sh_obj);
		}
		else
		{
			//Object* ptr_obj = new Object(RenderEngine::resourceManager.m_complete_models[attributes.object_type],
			Object* ptr_obj = new Object(RenderEngine::resourceManager.m_complete_models[attributes.id],
										 glm::vec3{ stof(attributes.posx),
										 		    stof(attributes.posy),
										 		    stof(attributes.posz) },
										 stof(attributes.scale),
										 "", "");
			std::shared_ptr<Object> sh_obj(ptr_obj);
			m_objects.push_back(sh_obj);
		}
	}
}

struct collisionThread
{
	std::vector<std::shared_ptr<Object>>& red;
	std::vector<std::shared_ptr<Object>>& blue;

	uint32_t start;
	uint32_t end;

	std::mutex& mut;

	void(*func)(std::vector<std::shared_ptr<Object>>& red_units, std::vector<std::shared_ptr<Object>>& blue_units, uint32_t start, uint32_t end, std::mutex& mutex);


	collisionThread(std::vector<std::shared_ptr<Object>>& m_red_units, 
					std::vector<std::shared_ptr<Object>>& m_blue_units, 
					uint32_t _start, 
					uint32_t _end,
					void(*_func)(std::vector<std::shared_ptr<Object>>&, std::vector<std::shared_ptr<Object>>&, uint32_t, uint32_t, std::mutex&),
					std::mutex& _mutex) 
					: red(m_red_units), blue(m_blue_units), start(_start), end(_end), func(_func), mut(_mutex)
	{}

	void operator()()
	{
		func(red, blue, start, end, mut);
	}
};

void Game::collisionsDetected()
{
	
	// why here is no bag??
	auto checkCollisionUnitToUnit = [](std::vector<std::shared_ptr<Object>>& m_red_units,
									   std::vector<std::shared_ptr<Object>>& m_blue_units,
									   uint32_t start, uint32_t end, 
									   std::mutex& blocker)
	{
		for (uint32_t i = start; i < end; i++)
		{
			Unit* unit_i = (Unit*)m_red_units[i].get();

			for (uint32_t j = 0; j < m_blue_units.size(); j++)
			{
				Unit* unit_j = (Unit*)m_blue_units[j].get();

				if (!unit_i->m_annihilated && 
					!unit_j->m_annihilated &&
					closerThan(unit_i->m_position, unit_j->m_position, unit_i->m_radius + unit_j->m_radius))
				{
					std::lock_guard<std::mutex> block(blocker);

					unit_i->m_annihilated = true;
					unit_j->m_annihilated = true;

					/*
					m_red_units.erase(m_red_units.begin() + i);
					m_blue_units.erase(m_blue_units.begin() + j);
					i--;
					j--;
					*/
				}
			}
		}
	};

	std::mutex mutex;

	uint32_t part = m_red_units.size() / 4;

	collisionThread colT1(m_red_units, m_blue_units, 0,			part,		  checkCollisionUnitToUnit, mutex);
	collisionThread colT2(m_red_units, m_blue_units, part, part * 2,		  checkCollisionUnitToUnit, mutex);
	collisionThread colT3(m_red_units, m_blue_units, part * 2, part * 3,		  checkCollisionUnitToUnit, mutex);
	collisionThread colT4(m_red_units, m_blue_units, part * 3, part * 4,		  checkCollisionUnitToUnit, mutex);

	std::thread t1(colT1);
	std::thread t2(colT2);
	std::thread t3(colT3);
	std::thread t4(colT4);

	t1.join();
	t2.join();
	t3.join();
	t4.join();

	auto checkCollisionUnitsPlanet = [](std::vector<std::shared_ptr<Object>>& objects, Planet* planet)
	{
		for (int j = 0; j < objects.size(); j++)
		{
			Unit* unit = (Unit*)objects[j].get();

			if (unit->m_targetPos == planet->m_position &&
				closerThan(unit->m_position, planet->m_position, unit->m_radius + planet->m_radius))
			{
				if (unit->m_state == "update" && unit->m_fraction == planet->m_fraction)
				{
					planet->plusOne();
					objects.erase(objects.begin() + j);
					j--;
				}

				if (unit->m_state == "attack" && unit->m_fraction != planet->m_fraction)
				{
					planet->minusOne(unit->m_fraction);
					objects.erase(objects.begin() + j);
					j--;
				}
			}
		}
	};

	for (uint32_t i = 0; i < m_objects.size(); i++)
	{
		if (m_objects[i]->m_type == Object::ObjectType::PLANET)
		{
			Planet* planet = (Planet*)m_objects[i].get();

			checkCollisionUnitsPlanet(m_red_units, planet);
			checkCollisionUnitsPlanet(m_blue_units, planet);
		}
	}

	for (uint32_t i = 0; i < m_red_units.size(); i++)
	{
		Unit* unit_i = (Unit*)m_red_units[i].get();

		if (unit_i->m_annihilated)
		{
			m_red_units.erase(m_red_units.begin() + i);
		}
	}
	for (uint32_t i = 0; i < m_blue_units.size(); i++)
	{
		Unit* unit_i = (Unit*)m_blue_units[i].get();

		if (unit_i->m_annihilated)
		{
			m_blue_units.erase(m_blue_units.begin() + i);
		}
	}
}

static int phys_counter = 0;
static int parts = 1;
static uint32_t lastEnd;
static bool lastEpochRed = true;
static bool lastEpochBlue = true;

void Game::updatePhysics()
{

	auto getForce = [](Unit* lhs, Unit* rhs)
	{
		if (lhs->m_position == rhs->m_position)
		{
			lhs->m_position = lhs->m_lastPos;
			rhs->m_position = rhs->m_lastPos;
		}

		float dist = glm::distance(lhs->m_position, rhs->m_position);
		float G = 0.8f;
		return G * 1.0f / sqrt(dist); // dist * dist - not work ?????;
	};

	auto setOffset = [](Unit* lhs, Unit* rhs, float force)
	{
		glm::vec3 temp = glm::normalize(lhs->m_position - rhs->m_position) * force;

		temp.y = 0.0f;

		lhs->m_gravityOffset += temp;
		rhs->m_gravityOffset -= temp;
	};

	auto updatePhysicUnitsSameFraction = [&getForce, &setOffset](Unit* lhs, Unit* rhs)
	{
		float force = getForce(lhs, rhs);
		if (force < 0.55f)
			force = 0.0f;
		setOffset(lhs, rhs, force);
	};

	auto updatePhysicUnitsDifferentFraction = [&getForce, &setOffset](Unit* lhs, Unit* rhs)
	{
		float force = getForce(lhs, rhs);
		if (force > 0.27f)
			force = -(force * 4.0f);
		else
			force = 0.0f;
		setOffset(lhs, rhs, force);
	};


	//uint32_t limit = m_red_units.size() / parts;
	//uint32_t counter = 0;
	uint32_t i = 0;

	phys_counter = (phys_counter + 1) % parts;
	uint32_t partSize = m_red_units.size() / parts;
	uint32_t start = partSize * phys_counter;
	uint32_t end = start + partSize;

	//for (i = 0; i < m_red_units.size() && counter <= limit; i++)
	for (i = start; i < end; i++)
	{
		Unit* unit_i = (Unit*)m_red_units[i].get();

		//if (unit_i->m_physUpdated != lastEpochRed)
		{
		//	counter++;

			for (uint32_t j = i + 1; j < m_red_units.size(); j++)
				updatePhysicUnitsSameFraction((Unit*)m_red_units[i].get(), (Unit*)m_red_units[j].get());

		//	unit_i->m_physUpdated = lastEpochRed;
		}
	}
	//if (i == m_red_units.size())
	//	lastEpochRed = !lastEpochRed;

	//limit = m_blue_units.size() / parts;
	//counter = 0;
	i = 0;

	phys_counter = (phys_counter + 1) % parts;
	partSize = m_blue_units.size() / parts;
	start = partSize * phys_counter;
	end = start + partSize;

	//for (i = 0; i < m_blue_units.size() && counter <= limit; i++)
	for (i = start; i < end; i++)
	{
		Unit* unit_i = (Unit*)m_blue_units[i].get();
		for (uint32_t j = i + 1; j < m_blue_units.size(); j++)
			updatePhysicUnitsSameFraction((Unit*)m_blue_units[i].get(), (Unit*)m_blue_units[j].get());
	}


	/*
	limit = m_red_units.size() / parts;
	counter = 0;

	for (uint32_t i = 0; i < m_red_units.size() && counter < limit; i++)
	{
		Unit* unit_i = (Unit*)m_red_units[i].get();

		if (!unit_i->m_physUpdated2)
		{
			counter++;

			for (uint32_t j = 0; j < m_blue_units.size(); j++)
				updatePhysicUnitsDifferentFraction((Unit*)m_red_units[i].get(), (Unit*)m_blue_units[j].get());

			unit_i->m_physUpdated2 = true;
		}
	}
	*/

	/*
	phys_counter++;
	if (phys_counter >= parts)
	{
		phys_counter = 0;

		for (uint32_t i = 0; i < m_red_units.size(); i++)
		{
			Unit* unit_i = (Unit*)m_red_units[i].get();
			unit_i->m_physUpdated = false;
			unit_i->m_physUpdated2 = false;
		}
		for (uint32_t i = 0; i < m_blue_units.size(); i++)
		{
			Unit* unit_i = (Unit*)m_blue_units[i].get();
			unit_i->m_physUpdated = false;
			unit_i->m_physUpdated2 = false;
		}

	}
	*/
}

void Game::updateGameState(float deltaTime)
{
	{
		//LOG_DURATION("updatePhysics");
		//updatePhysics();
	}

	{
		LOG_DURATION("collisionsDetected");
		collisionsDetected();
	}

	{
		LOG_DURATION("action");
		for (uint32_t i = 0; i < m_objects.size(); i++)
		{
			m_objects[i]->action(deltaTime);
		}
		for (uint32_t i = 0; i < m_red_units.size(); i++)
		{
			m_red_units[i]->action(deltaTime);
		}

		for (uint32_t i = 0; i < m_blue_units.size(); i++)
		{
			m_blue_units[i]->action(deltaTime);
		}
	}

	selectUnits();
}

void Game::selectUnits()
{
	if (m_area != nullptr)
	{
		//if (m_player_fraction == "RED")
		{
			/*
			for (auto&& obj : m_objects)
			{
				if (obj->m_type == Object::ObjectType::UNIT)
				{
					Unit* unit = (Unit*)obj.get();
					if (glm::distance(m_area->m_position, unit->m_position) <= m_area->m_radius)
						unit->select();
					else
						unit->deselect();
				}
			}
			*/

			for (auto&& obj : m_red_units)
			{
				if (obj->m_type == Object::ObjectType::UNIT /*&& obj->m_fraction == m_player_fraction*/)
				{
					Unit* unit = (Unit*)obj.get();
					if (glm::distance(m_area->m_position, unit->m_position) <= m_area->m_radius)
						unit->select();
					else
						unit->deselect();
				}
			}
			for (auto&& obj : m_blue_units)
			{
				if (obj->m_type == Object::ObjectType::UNIT /*&& obj->m_fraction == m_player_fraction*/)
				{
					Unit* unit = (Unit*)obj.get();
					if (glm::distance(m_area->m_position, unit->m_position) <= m_area->m_radius)
						unit->select();
					else
						unit->deselect();
				}
			}
		}
	}
}

void Game::createUnit(const std::string& unitType, const glm::vec3& position, const glm::vec3& targetPosition, const std::string& fraction)
{
	Unit* ptr_unit = new Unit(RenderEngine::resourceManager.m_map_complete_models[unitType], position, targetPosition, fraction);

	//m_objects.push_back(std::move(std::shared_ptr<Unit>(ptr_unit)));
	if(unitType == "RED_UNIT")
		m_red_units.push_back(std::move(std::shared_ptr<Unit>(ptr_unit)));
	if(unitType == "BLUE_UNIT")
		m_blue_units.push_back(std::move(std::shared_ptr<Unit>(ptr_unit)));
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
		RenderEngine::camera->finalCameraPoint -= 20.0f;
	if (scroll == -1)
		RenderEngine::camera->finalCameraPoint += 20.0f;

	if (RenderEngine::camera->finalCameraPoint < 10)
		RenderEngine::camera->finalCameraPoint = 10;
	if (RenderEngine::camera->finalCameraPoint > 900)
		RenderEngine::camera->finalCameraPoint = 900;
	
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
			RenderEngine::startCursorPos = cursorCoords;
			RenderEngine::firstMouse = false;
		}

		glm::vec3 startP = RenderEngine::cursorCoordToWorldCoords(RenderEngine::startCursorPos);
		glm::vec3 crurrP = RenderEngine::cursorCoordToWorldCoords(cursorCoords);

		RenderEngine::startCursorPos = cursorCoords;

		RenderEngine::camera->Position += startP - crurrP;
	}

	processArea(buttons[0], cursorCoords);
}

void Game::processArea(const bool& leftButton, const glm::vec2& cursorCoords)
{
	static glm::vec2 lastCursorCoords;
	static bool leftButtonLastTime = false;
	static bool clickOnButton = false;

	//selected area and buttons hit
	if (leftButton)
	{
		if (!leftButtonLastTime)
			clickOnButton = checkButtonHits(cursorCoords, true);

		//if last click was not on button and cursor moved while holding down the button - update area
		if (m_area != nullptr)
		{
			if (!clickOnButton && cursorCoords != lastCursorCoords)
				m_area->updateArea(cursorCoords);
		}

		leftButtonLastTime = true;
	}
	else
	{
		if (!checkButtonHits(cursorCoords, false) && leftButtonLastTime)
		{
			if (m_area != nullptr)
			{
				if (m_area->m_exsist && m_area->m_fixed)
				{
					setTargetForSelectedUnits(cursorCoords);
					m_area->collapseArea();
				}
			}
		}

		if (m_area != nullptr)
			m_area->m_fixed = true;
		leftButtonLastTime = false;
	}

	lastCursorCoords = cursorCoords;
}


bool Game::checkButtonHits(const glm::vec2& cursorCoords, bool isPressed)
{
	bool result = false;

	float x = cursorCoords.x / RenderEngine::SCREEN.x;
	float y = cursorCoords.y / RenderEngine::SCREEN.y;

	for (auto&& obj : m_objects)
	{
		if (obj->m_type == Object::ObjectType::BUTTON)
		{
			Button* button = (Button*)obj.get();
			if (button->cursorIsOnButton(glm::vec2(x, y)))
			{
				result = true;

				if (isPressed)
				{
					button->setButtonDown();
					button->m_isPressed = true;
				}
				else if (button->m_isPressed)
				{
					button->setButtonUp();
					button->callCallbackFunction();
					/*If this button was a button in the menu, then after pressing it it may no longer exist*/
					if (button != nullptr)
						button->m_isPressed = false;
					else
						return false;
				}
			}
			else
			{
				button->m_isPressed = false;
				button->setButtonUp();
			}
		}
	}
	
	return result;
}

void Game::setTargetForSelectedUnits(const glm::vec2& cursorCoords)
{
	glm::vec3 targetPos = RenderEngine::cursorCoordToWorldCoords({ cursorCoords.x, cursorCoords.y });

	std::string planetFraction = "";
	glm::vec3 planetPos;
	float planetLevel = 0;
	float planetLevelMAX = 0;

	for (auto&& obj : m_objects)
	{
		if (obj->m_type == Object::ObjectType::PLANET)
		{
			Planet* planet = (Planet*)obj.get(); 
			
			if (closerThan(planet->m_position, targetPos, obj->m_radius)) // click was on planet
			{
				planetFraction = planet->m_fraction;
				planetLevel = planet->m_level;
				planetLevelMAX = planet->s_max_level;
				planetPos = planet->m_position;
			}
		}
	}

	/*
	for (auto&& obj : m_objects)
	{
		if (obj->m_type == Object::ObjectType::UNIT)
		{
			Unit* unit = (Unit*)obj.get();

			if (unit->m_selected)
			{
				if (planetFraction == "")
				{
					unit->m_state = "move";
					unit->setTargetPosition(targetPos);
				}
				else
				{
					if (planetFraction == unit->m_fraction)
					{
						if (planetLevel < planetLevelMAX)
						{
							unit->m_state = "update"; 
							unit->setTargetPosition(planetPos);
						}
						else
						{
							unit->m_state = "move";
							unit->setTargetPosition(targetPos);
						}
					}
					else
					{
						unit->m_state = "attack";
						unit->setTargetPosition(planetPos);
					}
				}
			}
		}
	}
	*/
	for (auto&& obj : m_red_units)
	{
		if (obj->m_type == Object::ObjectType::UNIT)
		{
			Unit* unit = (Unit*)obj.get();

			if (unit->m_selected)
			{
				if (planetFraction == "")
				{
					unit->m_state = "move";
					unit->setTargetPosition(targetPos);
				}
				else
				{
					if (planetFraction == unit->m_fraction)
					{
						if (planetLevel < planetLevelMAX)
						{
							unit->m_state = "update";
							unit->setTargetPosition(planetPos);
						}
						else
						{
							unit->m_state = "move";
							unit->setTargetPosition(targetPos);
						}
					}
					else
					{
						unit->m_state = "attack";
						unit->setTargetPosition(planetPos);
					}
				}
			}
		}
	}
	for (auto&& obj : m_blue_units)
	{
		if (obj->m_type == Object::ObjectType::UNIT)
		{
			Unit* unit = (Unit*)obj.get();

			if (unit->m_selected)
			{
				if (planetFraction == "")
				{
					unit->m_state = "move";
					unit->setTargetPosition(targetPos);
				}
				else
				{
					if (planetFraction == unit->m_fraction)
					{
						if (planetLevel < planetLevelMAX)
						{
							unit->m_state = "update";
							unit->setTargetPosition(planetPos);
						}
						else
						{
							unit->m_state = "move";
							unit->setTargetPosition(targetPos);
						}
					}
					else
					{
						unit->m_state = "attack";
						unit->setTargetPosition(planetPos);
					}
				}
			}
		}
	}
	while (false);
}

void switchPause(Game& game)
{
	game.m_pause = !game.m_pause;
}

void speedGameUp(Game& game)
{
	RenderEngine::m_timeKoef += 0.25f;
	if (RenderEngine::m_timeKoef > 2.0f)
		RenderEngine::m_timeKoef = 2.0f;
}

void speedGameDown(Game& game)
{
	RenderEngine::m_timeKoef -= 0.25f;
	if (RenderEngine::m_timeKoef < 0.25f)
		RenderEngine::m_timeKoef = 0.25f;
}



//TODO
/*
	- при расчёте физики можно пользоваться многопоточкой
	- проверку на конец матча и вывод графика
	- сделать редактор карт
	- сделать api для удобста использования ai
	- добавить границы карты
	- добавить бокс (фон)
	- разобраться с анимациями (пульсирование планет, увеличение и т.д.)

	Решено:
		- сделать псевдо-физику (бывало что вылетало исключение, но сейчас вроде нет)

		- новая оптимизация физики - брать только первую половину всех юнитов и при проходе по каждому записывать смещение в ОБА юнита а не только в текущий
	
		- какой то баг с выходом за пределы вектора во время столкновения юнитов

		- баг в коллизион детекшн (если кинуть толпа на толпу одинаковое количество с обоих сторон, то у одной стороны всё равно остаются юниты)

		- сделать шкалу хп для планеты (механика шкал немного не как в оригинале, но пока что сойдёт) 
		(в оригинале если две фракции одновременно атакуют планету, то они соревнуются друг с другом, 
		а тут они топят планету вместе и чй юнит нанесёт последний удар, тому и достаётся планета)

		- сделать шрифты (пока только цифры но пока что больше и не надо)

		- лоад скрин (совсем не то что я хотел, но по другому не получается)

		- сделать меню (кое как сделано)

		- баг при расчёте физики - некоторые юниты пропускались (возможно не решено)

		- интерфейс (хп планет, и ускоритель времени как в тотал вар)

		- баг с исчезновением юнитов когда их становится много
		  костыльно решено, проблема была при расчёте физики, когда два юнита оказывались в одной точке то расстояние между ними = 0.0f и 1.0f / 0.0f давало нанд
		  а так же если пытаться нормализовать вектор длиной 0.0f то результат - вектор с нанд

		- исправить баг с дёрганьем юнитов когда они стоят на месте (это из за того что время * скорость и получается большое значение и юнит перепрыгивает таргет)

		- баг, если направить юнитов не на апдейт своей планеты, а атаковать вражескую или нейтральную, то юниты просто исчезают

		- баг с неправильным таргетом у юнита
		(если юнита отправить В планету и его путь будет
		пролегать через другую планет, то юнит войдёт не в ту
		планету на которую был отправлен, а в ту которая
		лежит у него на пути)

		- интерфейс (кнопки)

		- баг с физикой (разлетаются по координате y)

		- сделать нормальное ООП
		
		- замерять производительность (частично сделанно)

		- общая оптимизация (сократить количество вложенных циклов, сделать свою функцию distance без извлечения корня, и.т.д.)

		- улучшить графику и добавить опенгл фитчи (например, отправлять в шейдер модельки не по одной а все сразу)

		- общая оптимизация (сократить количество вложенных циклов, сделать свою функцию distance без извлечения корня, и.т.д.)

		- m_selected находится в Object, а дожен быть только у Unit, это кривое проэктирование.
		  RenderEngine при отрисовке смотрит на поле m_selected, чтобы определить,
		  нужно ли отрисовывать белое выделение вокруг юнита, и это определяется прям в функции drawObjects.
		  Нужно сделать так, чтоб если юнит выделен, то он сам определяет как он должен быть отрисован, и тогда,
		  drawObjects будет отрисовывать все объёкты одинаковым способом (просто выводить все модели на которые
		  у объекта есть индесы).
*/