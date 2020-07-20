#include "Game.h"
#include "Profile.h"

Game::Game(unsigned int level)
{
	m_player_fraction = "RED";
	m_ai_fraction = "BLUE";

	std::vector<ObjectAttributes> objectsAttribs;
	RenderEngine::resourceManager.loadLevel(0, objectsAttribs);

	// temporary here
	// create models groups
	for (auto&& x : RenderEngine::resourceManager.modelIndex_shadersIndices)
		RenderEngine::modelsGroups.push_back({ std::get<2>(RenderEngine::resourceManager.models[x.first]).depth_test, false, 0, x.first, x.second, std::vector<glm::mat4>() });
	
	for (auto&& x : objectsAttribs)
		createObject(x);

	//create objects manually
	uint32_t id = RenderEngine::resourceManager.createObject(std::vector<float>(240), "../FirstGame/Resources/shaders/1.area_shader.vs", "../FirstGame/Resources/shaders/1.area_shader.fs");
	m_area = std::make_shared<select_area>(id);
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
		LOG_DURATION("--- main loop");

		{
			LOG_DURATION("calculate deltaTime");
			float currTime = RenderEngine::getCurrTime();
			RenderEngine::deltaTime = currTime - lastTime;
			lastTime = currTime;
		}


		{
			LOG_DURATION("process input");
			processInput(RenderEngine::keys, RenderEngine::buttons, RenderEngine::cursorCoords, RenderEngine::scroll);
		}

		{
			LOG_DURATION("clearScreen");
			RenderEngine::clearScreen();
		}

		{
			LOG_DURATION("updateCameraView");
			RenderEngine::updateCameraView();
		}

		{
			LOG_DURATION("updateGameState");
			updateGameState(RenderEngine::deltaTime);
		}

		{
			LOG_DURATION("genModelMatrices");
			RenderEngine::genModelMatrices(Game::objects);
		}

		{
			LOG_DURATION("drawObjects");
			RenderEngine::drawObjects(Game::objects);
		}

		{
			LOG_DURATION("updateScreen");
			RenderEngine::updateScreen();
		}

		{
			LOG_DURATION("pollEvents");
			RenderEngine::pollEvents();
		}


		std::cerr << "objects " << objects.size() << std::endl;

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
	}
}

void Game::createObject(const ObjectAttributes& attributes)
{
	if (attributes.exsist == "true")
	{
		if (attributes.object_type == "RED_UNIT")
		{
			float x = 0;
			float z = 0;
			//for (x = 0; x < 500; x++)
			{
			//	for (z = 0; z < 50; z++)
				{
					Unit* ptr_unit = new Unit(RenderEngine::resourceManager.m_complete_models[attributes.object_type],
											  glm::vec3{ stof(attributes.posx) + x,
											  		     stof(attributes.posy),
											  		     stof(attributes.posz) + z},
											  stof(attributes.scale),
											  "", "RED");
					std::shared_ptr<Object> sp_unit(ptr_unit);
					objects.push_back(sp_unit);
				}
			}
		}
		else if (attributes.object_type == "BLUE_UNIT")
		{
			Unit* ptr_unit = new Unit(RenderEngine::resourceManager.m_complete_models[attributes.object_type],
									  glm::vec3{ stof(attributes.posx),
											     stof(attributes.posy),
											     stof(attributes.posz) },
									  stof(attributes.scale),
									  "", "BLUE");
			std::shared_ptr<Object> sp_unit(ptr_unit);
			objects.push_back(sp_unit);
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
static int collDtctParts = 20;

void Game::collisionsDetected()
{
	bool flag = false;

	collDtct_counter = (collDtct_counter + 1) % collDtctParts;
	uint32_t partSize = objects.size() / collDtctParts;
	uint32_t start = partSize * collDtct_counter;
	uint32_t end = start + partSize;

	for (uint32_t i = start; i < end; i++)
	//for (uint32_t i = 0; i < objects.size(); i++)
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
						//if (objects[i]->m_type == Object::ObjectType::UNIT && objects[j]->m_type == Object::ObjectType::UNIT)
						if (objects[j]->m_type == Object::ObjectType::UNIT)
						{
							Unit* unit_j = (Unit*)objects[j].get();

							//if (!unit_i->m_annihilated && !unit_j->m_annihilated &&
							//	unit_i->m_fraction != unit_j->m_fraction)

							if (!unit_j->m_annihilated && 
								 unit_i->m_fraction != unit_j->m_fraction)
							{
								//if (glm::distance(objects[i]->m_position, objects[j]->m_position) < objects[i]->m_radius + objects[j]->m_radius)
								if (closerThan(unit_i->m_position, unit_j->m_position, unit_i->m_radius + unit_j->m_radius))
								{
									flag = true;

									unit_i->m_annihilated = true;
									unit_j->m_annihilated = true;
								}
							}
						}
						else
						{
							Planet* planet = (Planet*)(objects[i]->m_type == Object::ObjectType::PLANET ? objects[i].get() : objects[j].get());
							Unit*	unit = (Unit*)(objects[i]->m_type == Object::ObjectType::UNIT ? objects[i].get() : objects[j].get());

							if (unit->m_state == "update" &&
								unit->m_fraction == planet->m_fraction &&
								unit->m_targetPos == planet->m_position &&
								//glm::distance(unit->m_position, planet->m_position) < unit->m_radius + planet->m_radius) // Unit and Planet same color
								closerThan(unit->m_position, planet->m_position, unit->m_radius + planet->m_radius))
							{
								flag = true;
								unit->m_annihilated = true;

								planet->m_healthPoints += 1.0f;

								if (planet->m_healthPoints == 101.0f)
								{
									planet->m_healthPoints = 100.0f;
									planet->m_level += 1;
								}
							}

							if (unit->m_state == "attack" &&
								unit->m_fraction != planet->m_fraction &&
								//glm::distance(unit->m_position, planet->m_position) < unit->m_radius + planet->m_radius) // Unit and Planet different color
								closerThan(unit->m_position, planet->m_position, unit->m_radius + planet->m_radius))
							{
								flag = true;
								unit->m_annihilated = true;

								planet->m_healthPoints -= 1.0f;

								if (planet->m_fraction != "NEUTRAL")
								{
									if (planet->m_healthPoints == 0.0f)
									{
										planet->m_fraction = "NEUTRAL";

										planet->m_indexes_of_displayd_models.clear();
										planet->m_indexes_of_displayd_models.push_back(2);
									}
								}
								else // planet neutral
								{
									if (planet->m_healthPoints == -1.0f)
									{
										planet->m_fraction = m_player_fraction;
										planet->m_healthPoints = 1.0f;

										planet->m_indexes_of_displayd_models.clear();
										planet->m_indexes_of_displayd_models.push_back(0);
									}
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
static int parts = 20;

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

			//if (unit_i->m_physics_counter > 1)
			{
				//unit_i->m_physics_counter = 0;

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
			}
			//else
			{
				//unit_i->m_physics_counter++;
			}
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
	for (auto&& obj : objects)
	{
		if (obj->m_type == Object::ObjectType::UNIT && obj->m_fraction == m_player_fraction)
		{
			Unit* unit = (Unit*)obj.get();
			if (glm::distance(m_area->m_position, obj->m_position) <= m_area->radius)
			{
				unit->select();
			}
			else
			{
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
		RenderEngine::camera->Position.y -= 5.0f;
	if (scroll == -1)
		RenderEngine::camera->Position.y += 5.0f;

	if (RenderEngine::camera->Position.y < 10)
		RenderEngine::camera->Position.y = 10;
	if (RenderEngine::camera->Position.y > 1000)
		RenderEngine::camera->Position.y = 1000;

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

			std::string state = "move";

			for (auto&& obj : objects)
			{
				if (obj->m_type == Object::ObjectType::PLANET && glm::distance(obj->m_position, targetPos) < obj->m_radius)
				{
					if (obj->m_fraction == m_player_fraction)
						state = "update";
					else
						state = "attack";

					targetPos = obj->m_position;
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


inline bool Game::closerThan(const glm::vec3& firstPosition, const glm::vec3& secondPosition, const float& distance)
{
	float x = firstPosition.x - secondPosition.x;
	//float y = firstPosition.y - secondPosition.y;
	float z = firstPosition.z - secondPosition.z;

	//return (x * x + y * y + z * z) < distance * distance;
	return (x * x + z * z) < distance * distance;
}

//TODO
/*
	- добавить границы карты
	- добавить бокс (фон)


	- m_selected находится в Object, а дожен быть только у Unit, это кривое проэктирование.
		RenderEngine при отрисовке смотрит на поле m_selected, чтобы определить, 
		нужно ли отрисовывать белое выделение вокруг юнита, и это определяется прям в функции drawObjects.
		Нужно сделать так, чтоб если юнит выделен, то он сам определяет как он должен быть отрисован, и тогда, 
		drawObjects будет отрисовывать все объёкты одинаковым способом (просто выводить все модели на которые 
		у объекта есть индесы).

	- при передвижении толпа юнитов не стремится в одну точку, а двигаются как бы строем
	- при расчёте физики можно пользоваться многопоточкой
	- редактор карт?


	- разобраться с анимациями (пульсирование планет, увеличение и т.д.)
	- баг с исчезновением юнитов когда их становится много (вроде бы его уже нет)
	- интерфейс (хп планет, и ускоритель времени как в тотал вар)


	Решено:
		- баг с неправильным таргетом у юнита
		(если юнита отправить В планету и его путь будет
		пролегать через другую планет, то юнит войдёт не в ту
		планету на которую был отправлен, а в ту которая
		лежит у него на пути)

		- сделать нормальное ООП
		
		- замерять производительность (частично сделанно)

		- общая оптимизация (сократить количество вложенных циклов, сделать свою функцию distance без извлечения корня, и.т.д.)

		- улучшить графику и добавить опенгл фитчи (например, отправлять в шейдер модельки не по одной а все сразу)

		- общая оптимизация (сократить количество вложенных циклов, сделать свою функцию distance без извлечения корня, и.т.д.)
*/