#include "Game.h"
#include "Profile.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glad/glad.h>

#include <GLFW/glfw3native.h>

/*
struct func
{
	uint32_t& progress;
	uint32_t& level;
	Game& game;
	GLFWwindow* context;
	void(*function)(GLFWwindow* context, Game& game, uint32_t& level, uint32_t& progress);

	func(GLFWwindow* c, Game& g, uint32_t& l, uint32_t& p, void(*funct)(GLFWwindow*, Game&, uint32_t&, uint32_t&)) : context(c), game(g), level(l), progress(p), function(funct)
	{}

	void operator() ()
	{
		function(context, game, level, progress);
	}
};*/


static bool flag = false;
static uint32_t level = 1;

void Game::loadResources()
{
	/*
	auto func = [this]() 
	{
		uint32_t progress = 0;
		GLFWwindow* context = RenderEngine::loadScreenWin;
		//GLFWwindow* context = RenderEngine::window;

		glfwMakeContextCurrent(context);
		if (!glfwInit()) std::cout << "wrong!" << std::endl;

		createGame(level, progress);

		flag = true;
	};
	std::thread t(func);
	t.detach();
	*/

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

	/*
	uint32_t progress = 0;
	createGame(level, progress);*/

}
Game::~Game() {}

void startButton(Game& game)
{
	game.m_objects.clear();
	RenderEngine::resourceManager.clear();
	RenderEngine::modelsGroups.clear();

	/*
	uint32_t progress = 0;
	level = 0;
	game.createGame(level, progress);
	*/

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
	//m_timeKoef = 1.0f;

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
			//LOG_DURATION("updateGameState");
			updateGameState(RenderEngine::deltaTime);
		}

		{
			//LOG_DURATION("genModelMatrices");
			RenderEngine::genModelMatrices(Game::m_objects);
		}

		{
			//LOG_DURATION("drawObjects");
			RenderEngine::drawObjects(m_labels);
		}

		{
			//LOG_DURATION("updateScreen");
			RenderEngine::updateScreen();
		}

		{
			//LOG_DURATION("pollEvents");
			RenderEngine::pollEvents();
		}


		//std::cerr << "objects " << m_objects.size() << std::endl;
		/*
		{
			LOG_DURATION("Profile");
			if (RenderEngine::deltaTime > 1.0f / 59.0f)
			{
				counter++;
			}
			else
			{
				counter = 0;
			}

			if (counter > 100)
			{
				std::cerr << std::endl;
				std::cerr << "total objects " << m_objects.size() << std::endl;
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

void Game::createObject(const ObjectAttributes& attributes)
{
	if (attributes.exsist == "true")
	{
		if (attributes.object_type == "RED_UNIT")
		{
			for (int i = 0; i < 2; i++)
			{
				//Unit* ptr_unit = new Unit(RenderEngine::resourceManager.m_complete_models[attributes.object_type],
				Unit* ptr_unit = new Unit(RenderEngine::resourceManager.m_complete_models[attributes.id],
					glm::vec3{ stof(attributes.posx) - 50.0f,
							   stof(attributes.posy),
							   stof(attributes.posz) /*+ float(i)*/ },
					stof(attributes.scale),
					"", "RED");
				std::shared_ptr<Object> sp_unit(ptr_unit);
				m_objects.push_back(sp_unit);
			}
		}
		else if (attributes.object_type == "BLUE_UNIT")
		{
			for (int i = 0; i < 2; i++)
			{
				//Unit* ptr_unit = new Unit(RenderEngine::resourceManager.m_complete_models[attributes.object_type],
				Unit* ptr_unit = new Unit(RenderEngine::resourceManager.m_complete_models[attributes.id],
					glm::vec3{ stof(attributes.posx) + 50.0f,
							   stof(attributes.posy),
							   stof(attributes.posz) /*+ float(i)*/ },
					stof(attributes.scale),
					"", "BLUE");
				std::shared_ptr<Object> sp_unit(ptr_unit);
				m_objects.push_back(sp_unit);
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

static int collDtct_counter = 0;
static int collDtctParts = 10;

void Game::collisionsDetected()
{
	bool flag = false;

	collDtct_counter = (collDtct_counter + 1) % collDtctParts;
	uint32_t partSize = m_objects.size() / collDtctParts;
	uint32_t start = partSize * collDtct_counter;
	uint32_t end = start + partSize;

	//for (uint32_t i = start; i < end; i++)
	for (uint32_t i = 0; i < m_objects.size(); i++)
	{
		if (m_objects[i]->m_type == Object::ObjectType::UNIT)
		{
			Unit* unit_i = (Unit*)m_objects[i].get();

			if (!unit_i->m_annihilated)
			{
				for (uint32_t j = 0; j < m_objects.size(); j++)
				{
					if (i != j)
					{
						if (m_objects[j]->m_type == Object::ObjectType::UNIT)
						{
							Unit* unit_j = (Unit*)m_objects[j].get();

							if (!unit_j->m_annihilated && 
								 unit_i->m_fraction != unit_j->m_fraction)
							{
								if (closerThan(unit_i->m_position, unit_j->m_position, unit_i->m_radius + unit_j->m_radius))
								{
									flag = true;

									unit_i->m_annihilated = true;
									unit_j->m_annihilated = true;
								}
							}
						}
						else if (m_objects[j]->m_type == Object::ObjectType::PLANET)
						{
							Planet* planet = (Planet*)m_objects[j].get();

							if (unit_i->m_targetPos == planet->m_position &&
								closerThan(unit_i->m_position, planet->m_position, unit_i->m_radius + planet->m_radius))
							{
								if (unit_i->m_state == "update" && unit_i->m_fraction == planet->m_fraction)
								{
									flag = true;
									unit_i->m_annihilated = true;

									planet->plusOne();
								}

								if (unit_i->m_state == "attack" && unit_i->m_fraction != planet->m_fraction)
								{
									flag = true;
									unit_i->m_annihilated = true;

									planet->minusOne(unit_i->m_fraction);
								}
							}
						}
					}
				}
			}
		}
	}

	if (flag)
	{
		for (uint32_t iter = 0; iter < m_objects.size(); iter++)
		{
			if (m_objects[iter]->m_type == Object::ObjectType::UNIT)
			{
				Unit* unit = (Unit*)m_objects[iter].get();

				if (unit->m_annihilated)
				{
					m_objects.erase(m_objects.begin() + iter);
					iter--;
				}
			}
		}
	}
}

static int phys_counter = 0;
static int parts = 10;
static uint32_t lastEnd;

void Game::updatePhysics()
{
	try
	{
		phys_counter = (phys_counter + 1) % parts;
		uint32_t partSize = m_objects.size() / parts;

		if (phys_counter == 0)
			lastEnd = 0;

		uint32_t start = lastEnd;
		uint32_t end = start + partSize;
		lastEnd = end;
	
		//for (uint32_t i = 0; i < m_objects.size(); i++)
		for (uint32_t i = start; i < end; i++)
		{
			if (m_objects[i]->m_type == Object::ObjectType::UNIT)
			{
				Unit* unit_i = (Unit*)m_objects[i].get();

				glm::vec3 totoalGravityVector = { 0.0f, 0.0f, 0.0f };

				for (uint32_t j = 0; j < m_objects.size(); j++)
				{
					if (j != i &&
						m_objects[j]->m_type == Object::ObjectType::UNIT)
					{
						Unit* unit_j = (Unit*)m_objects[j].get();

						float dist = glm::distance(unit_i->m_position, unit_j->m_position);

						if (dist == 0.0f)
						{
							while (false);
						}
						else
						{
							float force = (1.0f / sqrt(dist)) * 0.207f;

							if (unit_i->m_fraction != unit_j->m_fraction)
							{
								//std::cout << "force " << force << std::endl;
								if (force < 0.08)
									force = 0.0f;
								else
									force = -(force * 10.0f);
							}
							else
							{
								if (force < 0.15)
									force = 0.0f;
							}

							glm::vec3 temp = glm::normalize(unit_i->m_position - unit_j->m_position) * force;

							totoalGravityVector += temp;
						}
					}
				}

				unit_i->m_gravityOffset = totoalGravityVector;

				if (unit_i->m_gravityOffset.y != 0.0f)
					unit_i->m_gravityOffset.y = 0.0f;
			}
		}
	}
	catch (...)
	{
		while (false);
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
	}

	selectUnits();
}

void Game::selectUnits()
{
	if (m_area != nullptr)
	{
		//if (m_player_fraction == "RED")
		{
			for (auto&& obj : m_objects)
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
	m_objects.push_back(std::move(std::shared_ptr<Unit>(ptr_unit)));
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

	/*
	glm::vec3 targetPos = RenderEngine::cursorCoordToWorldCoords({ cursorCoords.x, cursorCoords.y });

	std::string state = "move";

	for (auto&& obj : m_objects)
	{
		if (obj->m_type == Object::ObjectType::PLANET)
		{
			Planet* planet = (Planet*)obj.get();

			if (closerThan(planet->m_position, targetPos, obj->m_radius)) // click was on planet
			{
				if (planet->m_fraction != m_player_fraction)
					state = "attack";
				else 
				if (planet->m_level < planet->s_max_level)
					state = "update";

				targetPos = obj->m_position;
			}
		}
	}

	for (auto&& obj : m_objects)
	{
		if (obj->m_type == Object::ObjectType::UNIT)
		{
			Unit* unit = (Unit*)obj.get();
			if (unit->m_selected)
			{
				unit->setTargetPosition(targetPos);
				unit->m_state = state;
			}
		}
	}
	*/
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
	- ������� ����� �� ��� �������
	- ������� �������� ����
	- ������� api ��� ������� ������������� ai

	- ����� �� ��� � ������� �� ������� ������� �� ����� ������������ ������

	- ����� ����������� ������ - ����� ������ ������ �������� ���� ������ � ��� ������� �� ������� ���������� �������� � ��� ����� � �� ������ � �������

	- ������� ������-������

	- ��� ������� ������ ����� ������������ �������������

	- �������� �� ����� ����� � ����� �������

	- �������� ������� �����
	- �������� ���� (���)
	- ����������� � ���������� (������������� ������, ���������� � �.�.)

	������:

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