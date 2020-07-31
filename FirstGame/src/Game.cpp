#include "Game.h"
#include "Profile.h"

Game::Game(unsigned int level)
{
	m_player_fraction = "RED";
	m_ai_fraction = "BLUE";

	std::vector<ObjectAttributes> objectsAttribs;
	RenderEngine::loadLevel(0, objectsAttribs);
	
	for (auto&& x : objectsAttribs)
		createObject(x);

	//create objects manually
	uint32_t id = RenderEngine::resourceManager.createObject(std::vector<float>(240), "../FirstGame/Resources/shaders/1.area_shader.vs", "../FirstGame/Resources/shaders/1.area_shader.fs");
	m_area = std::make_shared<select_area>(id);

	m_pause = false;
	m_timeKoef = 1.0f;
}

Game::~Game()
{

}

static int counter = 0;

void Game::loop()
{
	float lastTime = RenderEngine::getCurrTime();

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

				RenderEngine::deltaTime *= m_timeKoef;
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
			RenderEngine::genModelMatrices(Game::objects);
		}

		{
			//LOG_DURATION("drawObjects");
			RenderEngine::drawObjects();
		}

		{
			//LOG_DURATION("updateScreen");
			RenderEngine::updateScreen();
		}

		{
			//LOG_DURATION("pollEvents");
			RenderEngine::pollEvents();
		}


		//std::cerr << "objects " << objects.size() << std::endl;
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
				std::cerr << "total objects " << objects.size() << std::endl;
				for (auto&& group : RenderEngine::modelsGroups)
				{
					std::cerr << "buffer " << group.buffer << " model_index " << group.model_index << " matrices.size " << group.matrices.size() << std::endl;
				}

				std::cerr << std::endl;
				std::cout << objects.size() << std::endl;
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
			//for (int i = 0; i < 1000; i++)
			{
				Unit* ptr_unit = new Unit(RenderEngine::resourceManager.m_complete_models[attributes.object_type],
					glm::vec3{ stof(attributes.posx) /*- 50.0f*/,
							   stof(attributes.posy),
							   stof(attributes.posz) /*+ float(i)*/ },
					stof(attributes.scale),
					"", "RED");
				std::shared_ptr<Object> sp_unit(ptr_unit);
				objects.push_back(sp_unit);
			}
		}
		else if (attributes.object_type == "BLUE_UNIT")
		{
			//for (int i = 0; i < 1000; i++)
			{
				Unit* ptr_unit = new Unit(RenderEngine::resourceManager.m_complete_models[attributes.object_type],
					glm::vec3{ stof(attributes.posx) /*+ 50.0f*/,
							   stof(attributes.posy),
							   stof(attributes.posz) /*+ float(i)*/ },
					stof(attributes.scale),
					"", "BLUE");
				std::shared_ptr<Object> sp_unit(ptr_unit);
				objects.push_back(sp_unit);
			}
		}
		else if (attributes.object_type == "RED_PLANET")
		{
			Planet* ptr_planet = new Planet(RenderEngine::resourceManager.m_complete_models[attributes.object_type],
											glm::vec3{ stof(attributes.posx),
													   stof(attributes.posy),
													   stof(attributes.posz) },
											stof(attributes.scale),
											"",
											this, "RED");
			std::shared_ptr<Object> sh_planet(ptr_planet);
			objects.push_back(sh_planet);
		}
		else if (attributes.object_type == "BLUE_PLANET")
		{
			Planet* ptr_planet = new Planet(RenderEngine::resourceManager.m_complete_models[attributes.object_type],
											glm::vec3{ stof(attributes.posx),
													   stof(attributes.posy),
													   stof(attributes.posz) },
											stof(attributes.scale),
											"",
											this, "BLUE");
			std::shared_ptr<Object> sh_planet(ptr_planet);
			objects.push_back(sh_planet);
		}
		else if (attributes.object_type == "NEUTRAL_PLANET")
		{
			Planet* ptr_planet = new Planet(RenderEngine::resourceManager.m_complete_models[attributes.object_type],
											glm::vec3{ stof(attributes.posx),
													   stof(attributes.posy),
													   stof(attributes.posz) },
											stof(attributes.scale),
											"",
											this, "NEUTRAL");
			std::shared_ptr<Object> sh_planet(ptr_planet);
			objects.push_back(sh_planet);
		}
		else if (attributes.object_type == "BUTTON")
		{
			Button* ptr_button = new Button(RenderEngine::resourceManager.m_complete_models[attributes.object_type],
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


			objects.push_back(sh_button);
		}
		else if (attributes.object_type == "CAMERA")
		{
			RenderEngine::camera = std::make_shared<Camera>(glm::vec3(stof(attributes.posx), stof(attributes.posy), stof(attributes.posz)));
		}
		else
		{
			Object* ptr_obj = new Object(RenderEngine::resourceManager.m_complete_models[attributes.object_type],
										 glm::vec3{ stof(attributes.posx),
										 		    stof(attributes.posy),
										 		    stof(attributes.posz) },
										 stof(attributes.scale),
										 "", "");
			std::shared_ptr<Object> sh_obj(ptr_obj);
			objects.push_back(sh_obj);
		}
	}
}

static int collDtct_counter = 0;
static int collDtctParts = 10;

void Game::collisionsDetected()
{
	bool flag = false;

	collDtct_counter = (collDtct_counter + 1) % collDtctParts;
	uint32_t partSize = objects.size() / collDtctParts;
	uint32_t start = partSize * collDtct_counter;
	uint32_t end = start + partSize;

	//for (uint32_t i = start; i < end; i++)
	for (uint32_t i = 0; i < objects.size(); i++)
	{
		if (objects[i]->m_type == Object::ObjectType::UNIT)
		{
			Unit* unit_i = (Unit*)objects[i].get();

			if (!unit_i->m_annihilated)
			{
				for (uint32_t j = 0; j < objects.size(); j++)
				{
					if (i != j)
					{
						if (objects[j]->m_type == Object::ObjectType::UNIT)
						{
							Unit* unit_j = (Unit*)objects[j].get();

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
						else if (objects[j]->m_type == Object::ObjectType::PLANET)
						{
							Planet* planet = (Planet*)objects[j].get();

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
		for (uint32_t iter = 0; iter < objects.size(); iter++)
		{
			if (objects[iter]->m_type == Object::ObjectType::UNIT)
			{
				Unit* unit = (Unit*)objects[iter].get();

				if (unit->m_annihilated)
				{
					objects.erase(objects.begin() + iter);
					iter--;
				}
			}
		}
	}
}

static int phys_counter = 0;
static int parts = 10;

void Game::updatePhysics()
{
	phys_counter = (phys_counter + 1) % parts;
	uint32_t partSize = objects.size() / parts;
	uint32_t start = partSize * phys_counter;
	uint32_t end = start + partSize;
	
	//for (uint32_t i = 0; i < objects.size(); i++)
	for (uint32_t i = start; i < end; i++)
	{
		if (objects[i]->m_type == Object::ObjectType::UNIT)
		{
			Unit* unit_i = (Unit*)objects[i].get();

			glm::vec3 totoalGravityVector = { 0.0f, 0.0f, 0.0f };

			for (uint32_t j = 0; j < objects.size(); j++)
			{
				if (j != i &&
					objects[j]->m_type == Object::ObjectType::UNIT)
				{
					Unit* unit_j = (Unit*)objects[j].get();

					float force = (1.0f / sqrt(glm::distance(unit_i->m_position, unit_j->m_position))) * 0.2f;

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
			
			unit_i->m_gravityOffset = totoalGravityVector;

			if (unit_i->m_gravityOffset.y != 0.0f)
				unit_i->m_gravityOffset.y = 0.0f;
		}
	}
}

void Game::updateGameState(float deltaTime)
{
	{
		//LOG_DURATION("updatePhysics");
		updatePhysics();
	}

	{
		//LOG_DURATION("collisionsDetected");
		collisionsDetected();
	}

	{
		//LOG_DURATION("action");
		for (uint32_t i = 0; i < objects.size(); i++)
		{
			objects[i]->action(deltaTime);
		}
	}

	selectUnits();
}

void Game::selectUnits()
{
	if (m_player_fraction == "RED")
	{
		for (auto&& obj : objects)
		{
			if (obj->m_type == Object::ObjectType::UNIT && obj->m_fraction == m_player_fraction)
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

void Game::createUnit(const std::string& unitType, const glm::vec3& position, const glm::vec3& targetPosition, const std::string& fraction)
{
	Unit* ptr_unit = new Unit(RenderEngine::resourceManager.m_complete_models[unitType], position, targetPosition, fraction);
	objects.push_back(std::move(std::shared_ptr<Unit>(ptr_unit)));
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


	static glm::vec2 lastCursorCoords;
	static bool leftButtonLastTime = false;
	static bool clickOnButton = false;

	//selected area and buttons hit
	if (buttons[0])
	{
		if (!leftButtonLastTime)
			clickOnButton = checkButtonHits(cursorCoords, true);

		//if last click was not on button and cursor moved while holding down the button - update area
		if (!clickOnButton && cursorCoords != lastCursorCoords) 
			m_area->updateArea(cursorCoords);

		leftButtonLastTime = true;
	}
	else
	{
		if (!checkButtonHits(cursorCoords, false) && leftButtonLastTime)
		{
			if (m_area->m_exsist && m_area->m_fixed)
			{
				setTargetForSelectedUnits(cursorCoords);
				m_area->collapseArea();
			}
		}

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

	for (auto&& obj : objects)
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
					button->callCallbackFunction();
					button->m_isPressed = false;
				}
			}
			else
				button->m_isPressed = false;

			if (!isPressed)
				button->setButtonUp();
		}
	}
	
	return result;
}

void Game::setTargetForSelectedUnits(const glm::vec2& cursorCoords)
{
	glm::vec3 targetPos = RenderEngine::cursorCoordToWorldCoords({ cursorCoords.x, cursorCoords.y });

	std::string state = "move";

	for (auto&& obj : objects)
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

	for (auto&& obj : objects)
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
}

void switchPause(Game& game)
{
	game.m_pause = !game.m_pause;
}

void speedGameUp(Game& game)
{
	game.m_timeKoef += 0.25f;
	if (game.m_timeKoef > 2.0f)
		game.m_timeKoef = 2.0f;
}

void speedGameDown(Game& game)
{
	game.m_timeKoef -= 0.25f;
	if (game.m_timeKoef < 0.25f)
		game.m_timeKoef = 0.25f;
}

//TODO
/*
	- сделать меню
	- сделать редактор карт
	- лоад скрин
	- сделать api для удобста использования ai
	- добавить границы карты
	- добавить бокс (фон)

	- исправить баг с дёрганьем юнитов когда они стоят на месте (это из за того что время * скорость и получается большое значение и юнит перепрыгивает таргет)

	- при расчёте физики можно пользоваться многопоточкой


	- разобраться с анимациями (пульсирование планет, увеличение и т.д.)
	- баг с исчезновением юнитов когда их становится много (вроде бы его уже нет)
	- интерфейс (хп планет, и ускоритель времени как в тотал вар)


	Решено:
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