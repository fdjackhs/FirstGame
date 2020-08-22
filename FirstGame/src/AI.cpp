#include "Game.h"
#include "Utilities.h"

void Game::artificialInteligence()
{
	uint32_t my_numb_of_units = m_blue_units.size();
	uint32_t my_production_speed = 0;

	uint32_t enemy_numb_of_units = m_red_units.size();
	uint32_t enemy_production_speed = 0;

	std::vector<Planet*> my_planets;
	std::vector<Planet*> enemy_planets;
	std::vector<Planet*> neutral_planets;

	for (auto&& obj : m_objects)
	{
		if (obj->m_type == Object::ObjectType::PLANET && obj->m_fraction == "BLUE")
		{
			Planet* planet = (Planet*)obj.get();
			my_planets.push_back(planet);
			my_production_speed += uint32_t(planet->m_level);
		}
		if (obj->m_type == Object::ObjectType::PLANET && obj->m_fraction == "RED")
		{
			Planet* planet = (Planet*)obj.get();
			enemy_planets.push_back(planet);
			enemy_production_speed += uint32_t(planet->m_level);
		}
		if (obj->m_type == Object::ObjectType::PLANET && obj->m_fraction == "NEUTRAL")
		{
			Planet* planet = (Planet*)obj.get();
			neutral_planets.push_back(planet);
		}
	}

	//выбрать планету самую близкую к след левлу
	Planet* closestToUpdatePlanet = nullptr;

	if (my_production_speed <= enemy_production_speed || float(my_numb_of_units - 100) / float(enemy_numb_of_units) >= 1.0f)
	{
		//мы остаём (или равны но у нас достаточно уюнитов чтоб апгрейднуть планету) по производительности - нужно апгрейднуть планету

		for (uint32_t i = 0; i < my_planets.size(); i++)
		{
			if (my_planets[i]->m_level == my_planets[i]->s_max_level)
			{
				my_planets.erase(my_planets.begin() + i);
				i--;
			}
		}

		std::sort(my_planets.begin(), my_planets.end(), [](const Planet* lhs, const Planet* rhs)
		{
			if (lhs->m_upgradePoints > rhs->m_upgradePoints)
				return true;
			return false;
		});

		if (my_planets.size() == 0)
		{
			if (neutral_planets.size() > 0)
			{
				std::sort(neutral_planets.begin(), neutral_planets.end(), [&](const Planet* lhs, const Planet* rhs)
				{
					if (sqrt_distance(lhs->m_position, neutral_planets[0]->m_position) < sqrt_distance(rhs->m_position, neutral_planets[0]->m_position))
						return true;
					return false;
				});
				closestToUpdatePlanet = neutral_planets[0];
			}
		}
		else
		{
			closestToUpdatePlanet = my_planets[0];
		}

		if (closestToUpdatePlanet != nullptr)
		{
			uint32_t requiredNumberOfUnits = 100 - uint32_t(closestToUpdatePlanet->m_upgradePoints);
			if (my_numb_of_units > requiredNumberOfUnits)
			{
				if (float(my_numb_of_units) / float(enemy_numb_of_units) >= 1.0f) //соотношение сил в мою пользу или соотношение сил равное
				{
					std::sort(m_blue_units.begin(), m_blue_units.end(), [&closestToUpdatePlanet](const std::shared_ptr<Object> lhs, const std::shared_ptr<Object> rhs)
					{
						if (sqrt_distance(lhs->m_position, closestToUpdatePlanet->m_position) < sqrt_distance(rhs->m_position, closestToUpdatePlanet->m_position))
							return true;
						return false;
					});

					//взять достаточное количество юнитов из самых близлежащих к выбраной планете
					for (uint32_t i = 0; i < requiredNumberOfUnits; i++)
					{
						Unit* unit = (Unit*)m_blue_units[i].get();
						unit->m_selected = true;
					}

					//отправить
					//setTarget(m_blue_units, closestToUpdatePlanet->m_position, closestToUpdatePlanet->m_position, closestToUpdatePlanet->m_fraction, closestToUpdatePlanet->m_level, closestToUpdatePlanet->s_max_level);
					setTarget(m_blue_units, closestToUpdatePlanet->m_position, closestToUpdatePlanet);
				}
			}
		}
	}

	if (closestToUpdatePlanet == nullptr)
	{
		if (my_production_speed > enemy_production_speed && float(my_numb_of_units) / float(enemy_numb_of_units) >= 1.0f)
		{
			//идём в атаку

			//выбрать близжайшую планету для захвата (в идеале нужно смотреть и на хп планет)
			//но только если мы только что не отправили юнитов на захват нейтральной планеты

			Planet* closestToAttackPlanet = nullptr;
			std::sort(enemy_planets.begin(), enemy_planets.end(), [](const Planet* lhs, const Planet* rhs)
			{
				if (lhs->m_upgradePoints > rhs->m_upgradePoints && lhs->m_level != lhs->s_max_level)
					return true;
				return false;
			});
			closestToAttackPlanet = enemy_planets[0];

			//взять количество юнитов достаточное для захватаа планеты (выбрать самых близких к планете) + разницу (количество моих юнитов - его юниты)
			std::sort(m_blue_units.begin(), m_blue_units.end(), [&closestToAttackPlanet](const std::shared_ptr<Object> lhs, const std::shared_ptr<Object> rhs)
			{
				if (sqrt_distance(lhs->m_position, closestToAttackPlanet->m_position) < sqrt_distance(rhs->m_position, closestToAttackPlanet->m_position))
					return true;
				return false;
			});

			uint32_t requiredNumberOfUnits = m_blue_units.size() > 100 ? m_blue_units.size() - 1 : 100;

			//взять достаточное количество юнитов из самых близлежащих к выбраной планете
			for (uint32_t i = 0; i < requiredNumberOfUnits; i++)
			{
				Unit* unit = (Unit*)m_blue_units[i].get();
				unit->m_selected = true;
			}

			//отправить
			//setTarget(m_blue_units, closestToAttackPlanet->m_position, closestToAttackPlanet->m_position, closestToAttackPlanet->m_fraction, closestToAttackPlanet->m_level, closestToAttackPlanet->s_max_level);
			setTarget(m_blue_units, closestToAttackPlanet->m_position, closestToAttackPlanet);
		}
	}
}