#include "Game.h"
#include "Profile.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glad/glad.h>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

Game::Game(uint32_t level)
{
	loadResources(level);
}

Game::~Game() 
{
	freeResources();
}

void Game::loadResources(uint32_t level)
{
	RenderEngine::clearScreen();
	RenderEngine::updateCameraView();

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.5f, 1.0f, 1.0f));

	RenderEngine::resourceManager.m_loadScreenShader->use();
	RenderEngine::resourceManager.m_loadScreenShader->setMat4("model", model);
	RenderEngine::resourceManager.m_loadScreen->Draw(*RenderEngine::resourceManager.m_loadScreenShader);
	RenderEngine::updateScreen();

	uint32_t progress = 0;
	createGame(level, progress);
}

void Game::createGame(uint32_t& level, uint32_t& progress)
{
	m_player_fraction = "RED";
	m_ai_fraction = "BLUE";
	m_winner = "";
	m_area = nullptr;
	m_edgesOfMap = nullptr;
	m_inHand = nullptr;
	m_run = false;
	m_generateStar = false;
	m_inMapEditor = level == 2;
	m_pause = !m_inMapEditor;

	processLeftButton = processArea;
	selectOblects = (m_inMapEditor ? selectPlanets : selectUnits);

	Statistic* stat = new Statistic();
	m_statistic = std::shared_ptr<Statistic>(stat);

	std::vector<ObjectAttributes> objectsAttribs;
	RenderEngine::loadLevel(level, objectsAttribs, progress);

	for (auto&& x : objectsAttribs)
		createObject(x);

	if (m_generateStar)
		generateWhiteStars();

	m_lastTime = RenderEngine::getCurrTime();
	RenderEngine::deltaTime = 0.0f;
	m_statistic->resetLastTime();
}

void Game::freeResources()
{
	for (auto&& obj : m_objects)
		obj.reset();
	m_objects.clear();

	m_red_units.clear();
	m_blue_units.clear();
	m_labels.clear();
	m_statistic->clear();

	m_statistic.reset();
	m_area.reset();

	RenderEngine::resourceManager.clear();
	RenderEngine::modelsGroups.clear();
}

static int counter = 0;

void Game::loop()
{
	m_lastTime = RenderEngine::getCurrTime();

	while (!RenderEngine::windowShouldClose())
	{
		//LOG_DURATION("--- main loop");
		{
			//LOG_DURATION("calculate deltaTime");
			float currTime = RenderEngine::getCurrTime();
			RenderEngine::deltaTime = currTime - m_lastTime;
			m_lastTime = currTime;
		}


		{
			//LOG_DURATION("process input");
			if (m_inMenu) 
				RenderEngine::scroll = 0;
			processInput(RenderEngine::keys, RenderEngine::buttons, RenderEngine::cursorCoords, RenderEngine::scroll);
		}

		{
			if (m_run && !m_pause && !m_inMapEditor)
				artificialInteligence();
		}

		{
			//LOG_DURATION("clearScreen");
			RenderEngine::clearScreen();
		}

		{
			//LOG_DURATION("updateCameraView");
			if (m_inMenu) 
				RenderEngine::camera->menuMove(RenderEngine::deltaTime);
			RenderEngine::updateCameraView();
		}

		{
			//LOG_DURATION("updateGameState");
			if (m_run && !m_pause && !m_inMapEditor)
				updateGameState(RenderEngine::deltaTime * RenderEngine::m_timeKoef);
			selectOblects(*this);
		}

		{
			//LOG_DURATION("genModelMatrices");
			RenderEngine::genModelMatrices(Game::m_objects, Game::m_red_units, Game::m_blue_units);
		}

		{
			//LOG_DURATION("drawObjects");
			RenderEngine::drawObjects(m_labels);
		}


		{
			if (!m_inMapEditor)
			{
				if (checkEndGame())
				{
					if (m_run)
					{
						switchVisibleLabelsAndButtonsForStatScreen();
						m_run = false;
					}
					drawStatisticScreen();
					//m_pause = true;
				}
				else
				{
					m_statistic->saveStatisticTimePoint(RenderEngine::deltaTime * RenderEngine::m_timeKoef, m_red_units, m_blue_units);
				}
			}
			else
			{
				if (m_inHand != nullptr)
				{
					m_inHand->m_position = RenderEngine::cursorCoordToWorldCoords(RenderEngine::cursorCoords);
				}
			}
		}

		{
			//LOG_DURATION("updateScreen");
			RenderEngine::updateScreen();
		}

		{
			//LOG_DURATION("pollEvents");
			RenderEngine::pollEvents();
		}
		
		//std::cerr << "objects " << m_objects.size() + m_red_units.size() + m_blue_units.size() << std::endl;

		/*
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
		*/
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
				}
			}
		}
	};

	std::mutex mutex;

	uint32_t numbThreads = 8;
	std::vector<std::thread> threads;

	uint32_t part = m_red_units.size() / numbThreads;

	for (uint32_t i = 0; i < numbThreads; i++)
	{
		uint32_t start = (part * i);
		uint32_t end = (i == numbThreads - 1) ? m_red_units.size() : ((part * i) + part);
		threads.push_back(std::thread(collisionThread(m_red_units, m_blue_units, start, end, checkCollisionUnitToUnit, mutex)));
	}

	for (uint32_t i = 0; i < threads.size(); i++)
		threads[i].join();

	auto checkCollisionUnitsPlanet = [](std::vector<std::shared_ptr<Object>>& objects, Planet* planet)
	{
		for (uint32_t j = 0; j < objects.size(); j++)
		{
			Unit* unit = (Unit*)objects[j].get();

			if (unit->m_targetPos == planet->m_position &&
				//closerThan(unit->m_position, planet->m_position, unit->m_radius + planet->m_radius) &&
				closerThan(unit->m_position, planet->m_position, unit->m_radius + planet->m_radius / 10.0f) &&
				!unit->m_annihilated)
			{
				if ((planet->m_level < planet->s_max_level || planet->m_capturePoints != 0) &&
					unit->m_state == "update" && unit->m_fraction == planet->m_fraction)
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

	auto eraseAnihilatedUnits = [](std::vector<std::shared_ptr<Object>>& units) 
	{
		for (uint32_t i = 0; i < units.size(); i++)
		{
			Unit* unit_i = (Unit*)units[i].get();

			if (unit_i->m_annihilated)
			{
				units.erase(units.begin() + i);
			}
		}
	};

	eraseAnihilatedUnits(m_red_units);
	eraseAnihilatedUnits(m_blue_units);
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


	phys_counter = (phys_counter + 1) % parts;
	uint32_t partSize = m_red_units.size() / parts;
	uint32_t start = partSize * phys_counter;
	uint32_t end = start + partSize;

	uint32_t i = 0;
	for (i = start; i < end; i++)
	{
		Unit* unit_i = (Unit*)m_red_units[i].get();

		for (uint32_t j = i + 1; j < m_red_units.size(); j++)
			updatePhysicUnitsSameFraction((Unit*)m_red_units[i].get(), (Unit*)m_red_units[j].get());
	}

	phys_counter = (phys_counter + 1) % parts;
	partSize = m_blue_units.size() / parts;
	start = partSize * phys_counter;
	end = start + partSize;

	i = 0;
	for (i = start; i < end; i++)
	{
		Unit* unit_i = (Unit*)m_blue_units[i].get();
		for (uint32_t j = i + 1; j < m_blue_units.size(); j++)
			updatePhysicUnitsSameFraction((Unit*)m_blue_units[i].get(), (Unit*)m_blue_units[j].get());
	}
}

void Game::updateGameState(float deltaTime)
{
	{
		//LOG_DURATION("updatePhysics");
		//updatePhysics();
	}

	{
		//LOG_DURATION("collisionsDetected");
		collisionsDetected();
	}

	{
		//LOG_DURATION("action");
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
}


void Game::createUnit(const std::string& unitType, const glm::vec3& position, const glm::vec3& targetPosition, const std::string& fraction)
{
	Unit* ptr_unit = new Unit(RenderEngine::resourceManager.m_map_complete_models[unitType], position, 1.0f, "", fraction, this);

	ptr_unit->m_targetPos = targetPosition;
	ptr_unit->m_state = "move";

	if(unitType == "RED_UNIT")
		m_red_units.push_back(std::move(std::shared_ptr<Unit>(ptr_unit)));
	if(unitType == "BLUE_UNIT")
		m_blue_units.push_back(std::move(std::shared_ptr<Unit>(ptr_unit)));
}


void Game::generateWhiteStars()
{
	uint32_t nearWall = 300;
	uint32_t furtherWall = 500;

	uint32_t max_x = 3000;
	uint32_t max_z = 3000;

	for (uint32_t i = 0; i < 6000; i++)
	{
		int temp = (rand() % furtherWall);
		float y = - float(temp + nearWall);
		float x = float(rand() % max_x) - float(max_x / 2);
		float z = float(rand() % max_z) - float(max_z / 2);;

		float scale = float(rand() % 130) / 100.0f;

		WhiteStar* ptr_star = new WhiteStar(RenderEngine::resourceManager.m_map_complete_models["WHITE_STAR"], glm::vec3{ x, y, z }, scale, "", "FRACTION");
		std::shared_ptr<Object> sh_star(ptr_star);
		m_objects.push_back(sh_star);
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

		if (m_edgesOfMap != nullptr)
		{
			if (RenderEngine::camera->Position.x < m_edgesOfMap->left)  RenderEngine::camera->Position.x = m_edgesOfMap->left;
			if (RenderEngine::camera->Position.x > m_edgesOfMap->right) RenderEngine::camera->Position.x = m_edgesOfMap->right;
			if (RenderEngine::camera->Position.z < m_edgesOfMap->bot)   RenderEngine::camera->Position.z = m_edgesOfMap->bot;
			if (RenderEngine::camera->Position.z > m_edgesOfMap->top)   RenderEngine::camera->Position.z = m_edgesOfMap->top;
		}
	}

	processLeftButton(buttons[0], cursorCoords, *this);
	//processArea(buttons[0], cursorCoords);
}

void processArea(const bool& leftButton, const glm::vec2& cursorCoords, Game& game)
{
	static glm::vec2 firstPushPoint;
	static glm::vec2 lastCursorCoords;
	static bool leftButtonLastTime = false;
	static bool clickOnButton = false;

	//selected area and buttons hit
	if (leftButton)
	{
		if (!leftButtonLastTime)
			clickOnButton = game.checkButtonHits(cursorCoords, true);

		//if last click was not on button and cursor moved while holding down the button - update area
		if (game.m_area != nullptr && game.m_run && game.m_inHand == nullptr)
		{
			if (glm::distance(cursorCoords, firstPushPoint) > 5.0f)
			{
				if (!clickOnButton && cursorCoords != lastCursorCoords)
				{
					game.m_area->updateArea(cursorCoords);
				}
			}
		}

		if (leftButtonLastTime == false)
			firstPushPoint = cursorCoords;
		leftButtonLastTime = true;
	}
	else
	{
		if (!game.checkButtonHits(cursorCoords, false) && leftButtonLastTime)
		{
			if (game.m_area != nullptr)
			{
				if (game.m_area->m_exsist && game.m_area->m_fixed)
				{
					game.setTargetForSelectedUnits(cursorCoords);
					game.m_area->collapseArea();
					game.m_area->m_fixed = false;
					game.m_area->m_exsist = false;
				}
			}

			if (game.m_inHand != nullptr)
			{
				game.m_inHand = nullptr;
			}
		}

		if (game.m_area != nullptr)
			game.m_area->m_fixed = true;
			
		leftButtonLastTime = false;
	}

	lastCursorCoords = cursorCoords;
}

bool Game::checkButtonHits(const glm::vec2& cursorCoords, bool isPressed)
{
	bool result = false;

	float x = cursorCoords.x / RenderEngine::SCREEN.x;
	float y = cursorCoords.y / RenderEngine::SCREEN.y;

	Button* pressedButton = nullptr;

	for (auto&& obj : m_objects)
	{
		if (obj->m_type == Object::ObjectType::BUTTON && obj->m_visible)
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
					pressedButton = button;
				}
			}
			else
			{
				button->m_isPressed = false;
				button->setButtonUp();
			}
		}
	}

	if (pressedButton != nullptr && pressedButton->m_isPressed)
	{
		pressedButton->m_isPressed = false;
		pressedButton->callCallbackFunction();
	}
	
	return result;
}

void Game::setTargetForSelectedUnits(const glm::vec2& cursorCoords)
{
	glm::vec3 targetPos = RenderEngine::cursorCoordToWorldCoords({ cursorCoords.x, cursorCoords.y });

	Planet* targetPlanet = nullptr;

	for (auto&& obj : m_objects)
	{
		if (obj->m_type == Object::ObjectType::PLANET)
		{
			Planet* planet = (Planet*)obj.get(); 
			
			if (closerThan(planet->m_position, targetPos, obj->m_radius)) // click was on planet
			{
				targetPlanet = planet;
			}
		}
	}

	setTarget(m_red_units, targetPos, targetPlanet);
	//setTarget(m_blue_units, targetPos, targetPlanet);
}

void Game::setTarget(std::vector<std::shared_ptr<Object>>& units, glm::vec3& targetPos, Planet* targetPlanet)
//void Game::setTarget(std::vector<std::shared_ptr<Object>>& units, glm::vec3& targetPos, glm::vec3& planetPos, std::string& planetFraction, float planetLevel, float planetLevelMAX)
{
	for (auto&& obj : units)
	{
		if (obj->m_type == Object::ObjectType::UNIT)
		{
			Unit* unit = (Unit*)obj.get();

			if (unit->m_selected)
			{
				if (targetPlanet == nullptr)
				{
					unit->m_state = "move";
					unit->m_targetPos = targetPos;
				}
				else
				{
					if (targetPlanet->m_fraction == unit->m_fraction)
					{
						//if (targetPlanet->m_level < targetPlanet->s_max_level)
						{
							unit->m_state = "update";
							unit->m_targetPos = targetPlanet->m_position;
						}
						/*else if (targetPlanet->m_capturePoints > 0)
						{
							unit->m_state = "update";
							unit->m_targetPos = targetPlanet->m_position;
						}
						else
						{
							unit->m_state = "move";
							unit->m_targetPos = targetPos;
						}*/
					}
					else
					{
						unit->m_state = "attack";
						unit->m_targetPos = targetPlanet->m_position;
					}
				}
			}
		}
	}
};

bool Game::checkEndGame()
{
	m_winner = "";

	for (auto&& obj : m_objects)
	{
		if (obj->m_type == Object::ObjectType::PLANET)
		{
			if (obj->m_fraction != "NEUTRAL")
			{
				if (m_winner == "")
				{
					m_winner = obj->m_fraction;
				}
				else
				{
					if (m_winner != obj->m_fraction)
						return false;
				}
			}
		}
	}

	if (m_winner != "")
		return true;
	else
		return false;
}

void Game::switchVisibleLabelsAndButtonsForStatScreen()
{
	for (auto&& obj : m_objects)
	{
		if (obj->m_type == Object::ObjectType::BUTTON)
		{
			Button* button = (Button*)obj.get();

			if (button->m_optionalProperties == "SPEED_DOWN" ||
				button->m_optionalProperties == "SPEED_UP" ||
				button->m_optionalProperties == "PAUSE")
			{
				button->m_visible = false;
			}
			if (button->m_optionalProperties == "MENU_BUTTON")
				button->m_visible = true;
		}
	}
	for (auto&& lab : m_labels)
	{
		Label* label = (Label*)lab.get();
		if (label->m_optionalProperties == "LABEL_ONE")
			label->m_visible = false;
		if (label->m_optionalProperties == "LABEL_TWO" || label->m_optionalProperties == "LABEL_THREE")
			label->m_visible = true;

		if (m_winner == "RED"  && label->m_optionalProperties == "RED_WON")
			label->m_visible = true;
		if (m_winner == "BLUE"  && label->m_optionalProperties == "BLUE_WON")
			label->m_visible = true;
	}
}

void Game::drawStatisticScreen()
{
	float botWindow = -0.7f;
	float topWindow =  0.7f;

	float leftWindow = -0.8f;
	float rightWindow = 0.8f;

	float wind_width  = rightWindow - leftWindow;
	float wind_height = topWindow   - botWindow;


	float maxTime = m_statistic->getMaxTime();
	float maxUnit = m_statistic->getMaxUnit();

	std::vector<std::pair<float, float>> forDraw = m_statistic->compressStatistic(m_statistic->m_red_units);
	//convert time and numbers of unit from -1.0f to 1.0f
	auto convertPoints = [&](std::vector<std::pair<float, float>> stat_points)
	{
		std::vector<std::pair<float, float>> temp;

		for (auto&& point : stat_points)
		{
			float x = (float(point.first) / (maxTime / wind_width)) - (wind_width / 2.0f);
			float y = (float(point.second) / (maxUnit / wind_height)) - (wind_height / 2.0f);
			temp.push_back({ x, y });
		}

		return temp;
	};
	forDraw = convertPoints(forDraw);

	for (uint32_t i = 1; i < forDraw.size(); i++)
	{
		draw2Dline(glm::vec2(forDraw[i - 1].first, forDraw[i - 1].second), glm::vec2(forDraw[i].first, forDraw[i].second), glm::vec3(1.0f, 0.0f, 0.0f), RenderEngine::resourceManager.m_shaders);
	}

	//------------------------------------------------------

	forDraw = m_statistic->compressStatistic(m_statistic->m_blue_units);
	forDraw = convertPoints(forDraw);

	for (uint32_t i = 1; i < forDraw.size(); i++)
	{
		draw2Dline(glm::vec2(forDraw[i - 1].first, forDraw[i - 1].second), glm::vec2(forDraw[i].first, forDraw[i].second), glm::vec3(0.0f, 0.0f, 1.0f), RenderEngine::resourceManager.m_shaders);
	}

	draw2Dline(glm::vec2(leftWindow, botWindow), glm::vec2(leftWindow, topWindow), glm::vec3(1.0f, 1.0f, 1.0f), RenderEngine::resourceManager.m_shaders);
	draw2Dline(glm::vec2(leftWindow, botWindow), glm::vec2(rightWindow, botWindow), glm::vec3(1.0f, 1.0f, 1.0f), RenderEngine::resourceManager.m_shaders);
}

void switchPause(Game& game)
{
	game.m_pause = !game.m_pause;

	for (auto&& label : game.m_labels)
	{
		if (label->m_optionalProperties == "PAUSE")
		{
			label->m_visible = game.m_pause;
			break;
		}
	}
}

void speedGameUp(Game& game)
{
	RenderEngine::m_timeKoef += 0.25f;
	if (RenderEngine::m_timeKoef > 6.0f)
		RenderEngine::m_timeKoef = 6.0f;
}

void speedGameDown(Game& game)
{
	RenderEngine::m_timeKoef -= 0.25f;
	if (RenderEngine::m_timeKoef < 0.25f)
		RenderEngine::m_timeKoef = 0.25f;
}

void toMenuButton(Game& game)
{
	game.freeResources();
	RenderEngine::resourceManager.clear();
	RenderEngine::modelsGroups.clear();
	game.loadResources(0);
	game.m_inMenu = true;
}

void startButton(Game& game)
{
	game.freeResources();
	game.loadResources(RenderEngine::resourceManager.m_listPathLevels.size() - 1);
	game.m_run = true;
	game.m_pause = false;
	game.m_inMenu = false;
}

void startMapEditor(Game& game)
{
	game.freeResources();
	game.loadResources(2);
	game.m_run = true;
	game.m_pause = false;
	game.m_inMenu = false;
}

void selectUnits(Game& game)
{
	if (game.m_area != nullptr)
	{
		//if (m_player_fraction == "RED")
		{
			for (auto&& obj : game.m_red_units)
			{
				if (obj->m_type == Object::ObjectType::UNIT)
				{
					Unit* unit = (Unit*)obj.get();
					if (glm::distance(game.m_area->m_position, unit->m_position) <= game.m_area->m_radius)
						unit->select();
					else
						unit->deselect();
				}
			}
			/*
			for (auto&& obj : game.m_blue_units)
			{
				if (obj->m_type == Object::ObjectType::UNIT)
				{
					Unit* unit = (Unit*)obj.get();
					if (glm::distance(game.m_area->m_position, unit->m_position) <= game.m_area->m_radius)
						unit->select();
					else
						unit->deselect();
				}
			}
			*/
		}
	}
}

void selectPlanets(Game& game)
{
	if (game.m_area != nullptr)
	{
		for (auto&& obj : game.m_objects)
		{
			if (obj->m_type == Object::ObjectType::PLANET)
			{
				Planet* planet = (Planet*)obj.get();
				if (glm::distance(game.m_area->m_position, planet->m_position) <= game.m_area->m_radius)
					planet->select();
				else
					planet->deselect();
			}
		}
	}
}

void deletePlanets(Game& game)
{
	for (uint32_t i = 0; i < game.m_objects.size(); i++)
	{
		if (game.m_objects[i]->m_type == Object::ObjectType::PLANET)
		{
			Planet* planet = (Planet*)game.m_objects[i].get();

			if (planet->m_selected)
			{
				game.m_objects.erase(game.m_objects.begin() + i);
				i--;
			}
		}
	}
	if (game.m_area != nullptr)
	{
		game.m_area->collapseArea();
	}
}

void createRedPlanet(Game& game)
{
	if (game.m_inHand == nullptr)
	{
		Planet* ptr_planet = new Planet(RenderEngine::resourceManager.m_map_complete_models["RED_PLANET"], RenderEngine::cursorCoordToWorldCoords(RenderEngine::cursorCoords), 1.0f, "", &game, "RED");
		game.m_inHand = ptr_planet;
		std::shared_ptr<Object> sh_planet(ptr_planet);

		game.m_objects.push_back(std::move(sh_planet));
	}
}

void createBluePlanet(Game& game)
{

	if (game.m_inHand == nullptr)
	{
		Planet* ptr_planet = new Planet(RenderEngine::resourceManager.m_map_complete_models["BLUE_PLANET"], RenderEngine::cursorCoordToWorldCoords(RenderEngine::cursorCoords), 1.0f, "", &game, "BLUE");
		game.m_inHand = ptr_planet;
		std::shared_ptr<Object> sh_planet(ptr_planet);

		game.m_objects.push_back(std::move(sh_planet));
	}
}

void createNeutralPlanet(Game& game)
{
	if (game.m_inHand == nullptr)
	{
		Planet* ptr_planet = new Planet(RenderEngine::resourceManager.m_map_complete_models["NEUTRAL_PLANET"], RenderEngine::cursorCoordToWorldCoords(RenderEngine::cursorCoords), 1.0f, "", &game, "NEUTRAL");
		game.m_inHand = ptr_planet;
		std::shared_ptr<Object> sh_planet(ptr_planet);

		game.m_objects.push_back(std::move(sh_planet));
	}
}

//TODO
/*
	- сделать ai

	- кнопка старт ведёт на выбор уровня а не на загрузку первого уровня


	Решено:
		- сделать редактор карт

		- в меню сделать кнопку редактора карт

		- отрефакторить

		- фуллскрин

		- добавить границы карты (вроде бы работает)

		- баг в коллизион детекшен - если юниты направлены в планету, но пока они добрались до планеты та стала максимальным уровнем и больше не доступна для прокачки юниты всё равно убиваются об эту планету

		- добавить бокс (фон)

		- проверку на конец матча и вывод графика
			доделать кнопки меню, подумать, нужно ли двигать камеру после финиша,
			а, и циферки сделать

		- при расчёте коллизий можно пользоваться многопоточкой

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