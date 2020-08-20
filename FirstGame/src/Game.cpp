#include "Game.h"
#include "Profile.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glad/glad.h>

#include <GLFW/glfw3native.h>


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
	m_pause = false;
	m_run = false;
	m_generateStar = false;

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
			//LOG_DURATION("clearScreen");
			RenderEngine::clearScreen();
		}

		{
			LOG_DURATION("updateCameraView");
			if (m_inMenu) 
				RenderEngine::camera->menuMove(RenderEngine::deltaTime);
			RenderEngine::updateCameraView();
		}

		{
			LOG_DURATION("updateGameState");
			if (m_run && !m_pause)
				updateGameState(RenderEngine::deltaTime * RenderEngine::m_timeKoef);
			selectUnits();
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
			if (checkEndGame())
			{
				m_run = false;
				m_pause = true;
				switchVisibleLabelsAndButtonsForStatScreen();
				drawStatisticScreen();
			}
			else
			{
				m_statistic->saveStatisticTimePoint(RenderEngine::deltaTime * RenderEngine::m_timeKoef, m_red_units, m_blue_units);
			}
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

void Game::createObject(ObjectAttributes& attributes)
{
	if (attributes.exsist == "true")
	{
		if (attributes.object_type == "RED_UNIT")
		{
			for (int i = 0; i < 50; i++)
			{
				Unit* ptr_unit = new Unit(RenderEngine::resourceManager.m_complete_models[attributes.id],
										  glm::vec3{ stof(attributes.posx) - 50.0f, stof(attributes.posy), stof(attributes.posz) + float(i) },
										  stof(attributes.scale),
										  "", "RED", this);
				std::shared_ptr<Object> sp_unit(ptr_unit);
				m_red_units.push_back(sp_unit);
			}
		}
		else if (attributes.object_type == "BLUE_UNIT")
		{
			for (int i = 0; i < 0; i++)
			{
				Unit* ptr_unit = new Unit(RenderEngine::resourceManager.m_complete_models[attributes.id],
										  glm::vec3{ stof(attributes.posx) + 50.0f, stof(attributes.posy), stof(attributes.posz) + float(i) },
										  stof(attributes.scale),
										  "", "BLUE", this);
				std::shared_ptr<Object> sp_unit(ptr_unit);
				m_blue_units.push_back(sp_unit);
			}
		}
		else if (attributes.object_type == "RED_PLANET")
		{
			Planet* ptr_planet = new Planet(RenderEngine::resourceManager.m_complete_models[attributes.id],
											glm::vec3{ stof(attributes.posx), stof(attributes.posy), stof(attributes.posz) },
											stof(attributes.scale),
											"", this, "RED");
			std::shared_ptr<Object> sh_planet(ptr_planet);
			m_objects.push_back(sh_planet);
		}
		else if (attributes.object_type == "BLUE_PLANET")
		{
			Planet* ptr_planet = new Planet(RenderEngine::resourceManager.m_complete_models[attributes.id],
											glm::vec3{ stof(attributes.posx), stof(attributes.posy), stof(attributes.posz) },
											stof(attributes.scale),
											"", this, "BLUE");
			std::shared_ptr<Object> sh_planet(ptr_planet);
			m_objects.push_back(sh_planet);
		}
		else if (attributes.object_type == "NEUTRAL_PLANET")
		{
			Planet* ptr_planet = new Planet(RenderEngine::resourceManager.m_complete_models[attributes.id],
											glm::vec3{ stof(attributes.posx), stof(attributes.posy), stof(attributes.posz) },
											stof(attributes.scale),
											"", this, "NEUTRAL");
			std::shared_ptr<Object> sh_planet(ptr_planet);
			m_objects.push_back(sh_planet);
		}
		else if (attributes.object_type == "WHITE_STAR")
		{
			m_generateStar = true;
		}
		else if (attributes.object_type == "BUTTON")
		{
			Button* ptr_button = new Button(RenderEngine::resourceManager.m_complete_models[attributes.id],
										    glm::vec3{ stof(attributes.posx), stof(attributes.posy), stof(attributes.posz) },
										    stof(attributes.scale),
											this,
											attributes.optionalProperties);
			std::shared_ptr<Button> sh_button(ptr_button);

			if (attributes.optionalProperties == "PAUSE")
				sh_button->setCallbackFunc(switchPause);
			if (attributes.optionalProperties == "SPEED_UP")
				sh_button->setCallbackFunc(speedGameUp);
			if (attributes.optionalProperties == "SPEED_DOWN")
				sh_button->setCallbackFunc(speedGameDown);
			if (attributes.optionalProperties == "BUTTON_START")
				sh_button->setCallbackFunc(startButton);
			if (attributes.optionalProperties == "BUTTON_CREATE_MAP")
				sh_button->setCallbackFunc(startMapEditor);
			if (attributes.optionalProperties == "MENU_BUTTON")
			{
				sh_button->setCallbackFunc(toMenuButton);
				sh_button->m_visible = false;
			}

			m_objects.push_back(sh_button);
		}
		else if (attributes.object_type == "LABEL")
		{
			Var trackedValue;
			if (attributes.optionalProperties == "LABEL_ONE")
				trackedValue = Var("float", &RenderEngine::m_timeKoef);
			if (attributes.optionalProperties == "LABEL_TWO")
				trackedValue = Var("int32_t", &m_statistic->m_maxUnits);
			if (attributes.optionalProperties == "LABEL_THREE")
				trackedValue = Var("int32_t", &m_statistic->m_maxTime);
			if (attributes.optionalProperties == "PAUSE")
				trackedValue = Var("int32_t", new int(0));
			if (attributes.optionalProperties == "RED_WON")
				trackedValue = Var("int32_t", new int(0));
			if (attributes.optionalProperties == "BLUE_WON")
				trackedValue = Var("int32_t", new int(0));

			Label* label = new Label(RenderEngine::resourceManager.m_complete_models[attributes.id],
									 trackedValue,
									 glm::vec3(stof(attributes.posx), stof(attributes.posy), stof(attributes.posz)), 
									 stof(attributes.scale),
									 attributes.optionalProperties);

			if (attributes.optionalProperties == "LABEL_ONE")
				label->m_visible = true;
			if (attributes.optionalProperties == "LABEL_TWO"   || 
				attributes.optionalProperties == "LABEL_THREE" || 
				attributes.optionalProperties == "PAUSE"	   ||
				attributes.optionalProperties == "RED_WON"	   || 
				attributes.optionalProperties == "BLUE_WON")
				label->m_visible = false;

			std::shared_ptr<Label> sh_obj(label);
			m_labels.push_back(sh_obj);
		}
		else if (attributes.object_type == "CAMERA")
		{
			//RenderEngine::camera.reset();
			//RenderEngine::camera = std::make_shared<Camera>(glm::vec3(stof(attributes.posx), stof(attributes.posy), stof(attributes.posz)));
			RenderEngine::camera->Position = glm::vec3(stof(attributes.posx), stof(attributes.posy), stof(attributes.posz));
			RenderEngine::camera->finalCameraPoint = stof(attributes.optionalProperties);
		}
		else if (attributes.object_type == "AREA")
		{
			//create objects manually
			uint32_t id = RenderEngine::resourceManager.createObject(std::vector<float>(240), "../FirstGame/Resources/shaders/1.area_shader.vs", "../FirstGame/Resources/shaders/1.area_shader.fs");
			m_area = std::make_shared<select_area>(id);
		}
		else if (attributes.object_type == "EDGES")
		{
			for(auto&& ch : attributes.optionalProperties) { if (ch == '\'') ch = '"'; };

			rapidjson::Document d;
			d.Parse(attributes.optionalProperties.c_str());

			rapidjson::Value& left  = d["left"];
			rapidjson::Value& right = d["right"];
			rapidjson::Value& top   = d["top"];
			rapidjson::Value& bot   = d["bot"];

			m_edgesOfMap = {left.GetFloat(), right.GetFloat(), top.GetFloat(), bot.GetFloat() };
		}
		else
		{
			Object* ptr_obj = new Object(RenderEngine::resourceManager.m_complete_models[attributes.id],
										 glm::vec3{ stof(attributes.posx), stof(attributes.posy), stof(attributes.posz) },
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
				closerThan(unit->m_position, planet->m_position, unit->m_radius + planet->m_radius) &&
				!unit->m_annihilated)
			{
				if (planet->m_level < planet->s_max_level &&
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

	/*
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
	*/
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

void Game::selectUnits()
{
	if (m_area != nullptr)
	{
		//if (m_player_fraction == "RED")
		{
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

		if (RenderEngine::camera->Position.x < m_edgesOfMap.left)  RenderEngine::camera->Position.x = m_edgesOfMap.left;
		if (RenderEngine::camera->Position.x > m_edgesOfMap.right) RenderEngine::camera->Position.x = m_edgesOfMap.right;
		if (RenderEngine::camera->Position.z < m_edgesOfMap.bot)   RenderEngine::camera->Position.z = m_edgesOfMap.bot;
		if (RenderEngine::camera->Position.z > m_edgesOfMap.top)   RenderEngine::camera->Position.z = m_edgesOfMap.top;
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
		if (m_area != nullptr && m_run)
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

	auto setTarget = [&](std::vector<std::shared_ptr<Object>>& units) 
	{
		for (auto&& obj : units)
		{
			if (obj->m_type == Object::ObjectType::UNIT)
			{
				Unit* unit = (Unit*)obj.get();

				if (unit->m_selected)
				{
					if (planetFraction == "")
					{
						unit->m_state = "move";
						unit->m_targetPos = targetPos;
					}
					else
					{
						if (planetFraction == unit->m_fraction)
						{
							if (planetLevel < planetLevelMAX)
							{
								unit->m_state = "update";
								unit->m_targetPos = planetPos;
							}
							else
							{
								unit->m_state = "move";
								unit->m_targetPos = targetPos;
							}
						}
						else
						{
							unit->m_state = "attack";
							unit->m_targetPos = planetPos;
						}
					}
				}
			}
		}
	};

	setTarget(m_red_units);
	setTarget(m_blue_units);
}

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
		draw2Dline(glm::vec2(forDraw[i - 1].first, forDraw[i - 1].second), glm::vec2(forDraw[i].first, forDraw[i].second), glm::vec3(1.0f, 0.0f, 0.0f), RenderEngine::resourceManager.m_shaders, *RenderEngine::camera);
	}

	//------------------------------------------------------

	forDraw = m_statistic->compressStatistic(m_statistic->m_blue_units);
	forDraw = convertPoints(forDraw);

	for (uint32_t i = 1; i < forDraw.size(); i++)
	{
		draw2Dline(glm::vec2(forDraw[i - 1].first, forDraw[i - 1].second), glm::vec2(forDraw[i].first, forDraw[i].second), glm::vec3(0.0f, 0.0f, 1.0f), RenderEngine::resourceManager.m_shaders, *RenderEngine::camera);
	}

	draw2Dline(glm::vec2(leftWindow, botWindow), glm::vec2(leftWindow, topWindow), glm::vec3(1.0f, 1.0f, 1.0f), RenderEngine::resourceManager.m_shaders, *RenderEngine::camera);
	draw2Dline(glm::vec2(leftWindow, botWindow), glm::vec2(rightWindow, botWindow), glm::vec3(1.0f, 1.0f, 1.0f), RenderEngine::resourceManager.m_shaders, *RenderEngine::camera);
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
	if (RenderEngine::m_timeKoef > 4.0f)
		RenderEngine::m_timeKoef = 4.0f;
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
	game.loadResources(3);
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



//TODO
/*
	- � ��������� ����
		- ������ ������� �������
		- ������ ����� �������
		- ������ ����������� �������
		- ���������
		- ����
		- ������ �������

	- ������ ����� ���� �� ����� ������ � �� �� �������� ������� ������

	- ������� �������� ����
	- ������� api ��� ������� ������������� ai

	������:
		- � ���� ������� ������ ��������� ����

		- �������������

		- ���������

		- �������� ������� ����� (����� �� ��������)

		- ��� � ��������� �������� - ���� ����� ���������� � �������, �� ���� ��� ��������� �� ������� �� ����� ������������ ������� � ������ �� �������� ��� �������� ����� �� ����� ��������� �� ��� �������

		- �������� ���� (���)

		- �������� �� ����� ����� � ����� �������
			�������� ������ ����, ��������, ����� �� ������� ������ ����� ������,
			�, � ������� �������

		- ��� ������� �������� ����� ������������ �������������

		- ������� ������-������ (������ ��� �������� ����������, �� ������ ����� ���)

		- ����� ����������� ������ - ����� ������ ������ �������� ���� ������ � ��� ������� �� ������� ���������� �������� � ��� ����� � �� ������ � �������
	
		- ����� �� ��� � ������� �� ������� ������� �� ����� ������������ ������

		- ��� � ��������� ������� (���� ������ ����� �� ����� ���������� ���������� � ����� ������, �� � ����� ������� �� ����� �������� �����)

		- ������� ����� �� ��� ������� (�������� ���� ������� �� ��� � ���������, �� ���� ��� �����) 
		(� ��������� ���� ��� ������� ������������ ������� �������, �� ��� ����������� ���� � ������, 
		� ��� ��� ����� ������� ������ � �� ���� ������ ��������� ����, ���� � �������� �������)

		- ������� ������ (���� ������ ����� �� ���� ��� ������ � �� ����)

		- ���� ����� (������ �� �� ��� � �����, �� �� ������� �� ����������)

		- ������� ���� (��� ��� �������)

		- ��� ��� ������� ������ - ��������� ����� ������������ (�������� �� ������)

		- ��������� (�� ������, � ���������� ������� ��� � ����� ���)

		- ��� � ������������� ������ ����� �� ���������� �����
		  ��������� ������, �������� ���� ��� ������� ������, ����� ��� ����� ����������� � ����� ����� �� ���������� ����� ���� = 0.0f � 1.0f / 0.0f ������ ����
		  � ��� �� ���� �������� ������������� ������ ������ 0.0f �� ��������� - ������ � ����

		- ��������� ��� � �������� ������ ����� ��� ����� �� ����� (��� �� �� ���� ��� ����� * �������� � ���������� ������� �������� � ���� ������������� ������)

		- ���, ���� ��������� ������ �� �� ������ ����� �������, � ��������� ��������� ��� �����������, �� ����� ������ ��������

		- ��� � ������������ �������� � �����
		(���� ����� ��������� � ������� � ��� ���� �����
		��������� ����� ������ ������, �� ���� ����� �� � ��
		������� �� ������� ��� ���������, � � �� �������
		����� � ���� �� ����)

		- ��������� (������)

		- ��� � ������� (����������� �� ���������� y)

		- ������� ���������� ���
		
		- �������� ������������������ (�������� ��������)

		- ����� ����������� (��������� ���������� ��������� ������, ������� ���� ������� distance ��� ���������� �����, �.�.�.)

		- �������� ������� � �������� ������ ����� (��������, ���������� � ������ �������� �� �� ����� � ��� �����)

		- ����� ����������� (��������� ���������� ��������� ������, ������� ���� ������� distance ��� ���������� �����, �.�.�.)

		- m_selected ��������� � Object, � ����� ���� ������ � Unit, ��� ������ ��������������.
		  RenderEngine ��� ��������� ������� �� ���� m_selected, ����� ����������,
		  ����� �� ������������ ����� ��������� ������ �����, � ��� ������������ ���� � ������� drawObjects.
		  ����� ������� ���, ���� ���� ���� �������, �� �� ��� ���������� ��� �� ������ ���� ���������, � �����,
		  drawObjects ����� ������������ ��� ������� ���������� �������� (������ �������� ��� ������ �� �������
		  � ������� ���� ������).
*/