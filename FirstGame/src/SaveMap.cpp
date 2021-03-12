#include "Game.h"

void saveMap(Game& game)
{
	std::vector<std::shared_ptr<Object>> planets;

	for (auto&& obj : game.m_objects)
	{
		if (obj->m_type == Object::ObjectType::PLANET)
		{
			planets.push_back(obj);
		}
	}

	uint32_t indexLeft = 0;
	uint32_t indexRight = 0;
	uint32_t indexBot = 0;
	uint32_t indexTop = 0;

	for (uint32_t i = 0; i < planets.size(); i++)
	{
		if (planets[i]->m_position.x < planets[indexLeft]->m_position.x)
			indexLeft = i;
		if (planets[i]->m_position.x > planets[indexRight]->m_position.x)
			indexRight = i;
		if (planets[i]->m_position.z < planets[indexBot]->m_position.z)
			indexBot = i;
		if (planets[i]->m_position.z > planets[indexTop]->m_position.z)
			indexTop = i;
	}

	uint32_t numLevel = RenderEngine::resourceManager.m_listPathLevels.size();
	std::string path = "./FirstGame/resources/ResourceManager/level" + std::to_string(numLevel) + ".txt";

	RenderEngine::resourceManager.m_listPathLevels.push_back(path);
	std::ofstream list_path_level;
	list_path_level.open("./FirstGame/Resources/ResourceManager/list_path_levels.txt", std::ios::app);
	list_path_level << "\n" << path;
	//list_path_level.flush();
	list_path_level.close();

	std::ofstream output;
	output.open(path);

	output << "[\n";
	output << "{\n";
	output << "\"object_type\": \"CAMERA\",\n";
	output << "\"models_type\": [],\n";
	output << "\"posx\": \"0.0\",\n";
	output << "\"posy\": \"0.0\",\n";
	output << "\"posz\": \"0.0\",\n";
	output << "\"scale\": \"1.0\",\n";
	output << "\"exsist\": \"true\",\n";
	output << "\"optionalProperties\": \"300.0\"\n";
	output << "}\n";
	output << ",\n";
	output << "{\n";
	output << "\"object_type\": \"EDGES\",\n";
	output << "\"models_type\": [],\n";
	output << "\"posx\": \"0.0\",\n";
	output << "\"posy\": \"0.0\",\n";
	output << "\"posz\": \"0.0\",\n";
	output << "\"scale\": \"1.0\",\n";
	output << "\"exsist\": \"true\",\n";
	output << "\"optionalProperties\": \"{ 'left' :" << std::to_string(planets[indexLeft]->m_position.x + -100.0f) <<
										", 'right': " << std::to_string(planets[indexRight]->m_position.x + 100.0f) <<
										", 'top' : " << std::to_string(planets[indexTop]->m_position.z + 100.0f) <<
										", 'bot' : " << std::to_string(planets[indexBot]->m_position.z + -100.0f) << "}\"\n";
	output << "}\n";
	output << ",\n";
	output << "{\n";
	output << "\"object_type\": \"AREA\",\n";
	output << "\"models_type\": [],\n";
	output << "\"posx\": \"0.0\",\n";
	output << "\"posy\": \"0.0\",\n";
	output << "\"posz\": \"0.0\",\n";
	output << "\"scale\": \"1.0\",\n";
	output << "\"exsist\": \"true\",\n";
	output << "\"optionalProperties\": \"AREA\"\n";
	output << "}\n";
	output << ",\n";
	output << "{\n";
	output << "\"object_type\": \"WHITE_STAR\",\n";
	output << "\"models_type\": [\"WHITE_STAR\"],\n";
	output << "\"posx\": \"0.0\",\n";
	output << "\"posy\": \"0.0\",\n";
	output << "\"posz\": \"0.0\",\n";
	output << "\"scale\": \"1.0\",\n";
	output << "\"exsist\": \"true\",\n";
	output << "\"optionalProperties\": \"WHITE_STAR\"\n";
	output << "}\n";

	for (uint32_t i = 0; i < planets.size(); i++)
	{
		output << ",\n";
		output << "{\n";
		output << "\"object_type\": \"" << planets[i]->m_fraction + "_PLANET\",\n";
		output << "\"models_type\": [ \"RED_PLANET\", \"BLUE_PLANET\", \"NEUTRAL_PLANET\", \"CAPTURE_SCALE\", \"UPGRADE_SCALE\" ],\n";
		output << "\"posx\": \"" << std::to_string(planets[i]->m_position.x) << "\",\n";
		output << "\"posy\": \"" << std::to_string(planets[i]->m_position.y) << "\",\n";
		output << "\"posz\": \"" << std::to_string(planets[i]->m_position.z) << "\",\n";
		output << "\"scale\": \"1.0\",\n";
		output << "\"exsist\": \"true\",\n";
		output << "\"optionalProperties\": \"" << planets[i]->m_fraction + "_PLANET\"\n";
		output << "}\n";
	}

	output << ",\n";
	output << "{\n";
	output << "\"object_type\": \"RED_UNIT\",\n";
	output << "\"models_type\": [\"STENCIL_UNIT\", \"RED_UNIT\"],\n";
	output << "\"posx\": \"0.0\",\n";
	output << "\"posy\": \"0.0\",\n";
	output << "\"posz\": \"0.0\",\n";
	output << "\"scale\": \"1.0\",\n";
	output << "\"exsist\": \"false\",\n";
	output << "\"optionalProperties\": \"RED_UNIT\"\n";
	output << "}\n";
	output << ",\n";
	output << "{\n";
	output << "\"object_type\": \"BLUE_UNIT\",\n";
	output << "\"models_type\": [\"STENCIL_UNIT\", \"BLUE_UNIT\"],\n";
	output << "\"posx\": \"0.0\",\n";
	output << "\"posy\": \"0.0\",\n";
	output << "\"posz\": \"0.0\",\n";
	output << "\"scale\": \"1.0\",\n";
	output << "\"exsist\": \"false\",\n";
	output << "\"optionalProperties\": \"BLUE_UNIT\"\n";
	output << "}\n";
	output << ",\n";
	output << "{\n";
	output << "\"object_type\": \"BUTTON\",\n";
	output << "\"models_type\": [\"BUTTON\", \"BUTTON_DOWN\"],\n";
	output << "\"posx\": \"0.925\",\n";
	output << "\"posy\": \"-0.85\",\n";
	output << "\"posz\": \"0.0\",\n";
	output << "\"scale\": \"0.1\",\n";
	output << "\"exsist\": \"true\",\n";
	output << "\"optionalProperties\": \"{ 'func' : 'PAUSE', 'visible' : 'true' }\"\n";
	output << "}\n";
	output << ",\n";
	output << "{\n";
	output << "\"object_type\": \"BUTTON\",\n";
	output << "\"models_type\": [\"BUTTON\", \"BUTTON_DOWN\"],\n";
	output << "\"posx\": \"0.93\",\n";
	output << "\"posy\": \"0.85\",\n";
	output << "\"posz\": \"0.0\",\n";
	output << "\"scale\": \"0.05\",\n";
	output << "\"exsist\": \"true\",\n";
	output << "\"optionalProperties\": \"{ 'func' : 'SPEED_UP', 'visible' : 'true' }\"\n";
	output << "}\n";
	output << ",\n";
	output << "{\n";
	output << "\"object_type\": \"BUTTON\",\n";
	output << "\"models_type\": [\"BUTTON\", \"BUTTON_DOWN\"],\n";
	output << "\"posx\": \"0.77\",\n";
	output << "\"posy\": \"0.85\",\n";
	output << "\"posz\": \"0.0\",\n";
	output << "\"scale\": \"0.05\",\n";
	output << "\"exsist\": \"true\",\n";
	output << "\"optionalProperties\": \"{ 'func' : 'SPEED_DOWN', 'visible' : 'true' }\"\n";
	output << "}\n";
	output << ",\n";
	output << "{\n";
	output << "\"object_type\": \"LABEL\",\n";
	output << "\"models_type\": [\"PAUSE\"],\n";
	output << "\"posx\": \"0.0\",\n";
	output << "\"posy\": \"0.0\",\n";
	output << "\"posz\": \"0.0\",\n";
	output << "\"scale\": \"0.35\",\n";
	output << "\"exsist\": \"true\",\n";
	output << "\"optionalProperties\": \"PAUSE\"\n";
	output << "}\n";
	output << ",\n";
	output << "{\n";
	output << "\"object_type\": \"LABEL\",\n";
	output << "\"models_type\": [\"RED_WON\"],\n";
	output << "\"posx\": \"0.0\",\n";
	output << "\"posy\": \"0.7\",\n";
	output << "\"posz\": \"0.0\",\n";
	output << "\"scale\": \"0.25\",\n";
	output << "\"exsist\": \"true\",\n";
	output << "\"optionalProperties\": \"RED_WON\"\n";
	output << "}\n";
	output << ",\n";
	output << "{\n";
	output << "\"object_type\": \"LABEL\",\n";
	output << "\"models_type\": [\"BLUE_WON\"],\n";
	output << "\"posx\": \"0.0\",\n";
	output << "\"posy\": \"0.7\",\n";
	output << "\"posz\": \"0.0\",\n";
	output << "\"scale\": \"0.25\",\n";
	output << "\"exsist\": \"true\",\n";
	output << "\"optionalProperties\": \"BLUE_WON\"\n";
	output << "}\n";
	output << ",\n";
	output << "{\n";
	output << "\"object_type\": \"LABEL\",\n";
	output << "\"models_type\": [\"0\", \"1\", \"2\", \"3\", \"4\", \"5\", \"6\", \"7\", \"8\", \"9\", \"COMMA\", \"DOT\"],\n";
	output << "\"posx\": \"0.8125\",\n";
	output << "\"posy\": \"0.85\",\n";
	output << "\"posz\": \"0.0\",\n";
	output << "\"scale\": \"0.05\",\n";
	output << "\"exsist\": \"true\",\n";
	output << "\"optionalProperties\": \"LABEL_ONE\"\n";
	output << "}\n";
	output << ",\n";
	output << "{\n";
	output << "\"object_type\": \"LABEL\",\n";
	output << "\"models_type\": [\"0\", \"1\", \"2\", \"3\", \"4\", \"5\", \"6\", \"7\", \"8\", \"9\", \"COMMA\", \"DOT\"],\n";
	output << "\"posx\": \"-0.9\",\n";
	output << "\"posy\": \"0.65\",\n";
	output << "\"posz\": \"0.0\",\n";
	output << "\"scale\": \"0.05\",\n";
	output << "\"exsist\": \"true\",\n";
	output << "\"optionalProperties\": \"LABEL_TWO\"\n";
	output << "}\n";
	output << ",\n";
	output << "{\n";
	output << "\"object_type\": \"LABEL\",\n";
	output << "\"models_type\": [\"0\", \"1\", \"2\", \"3\", \"4\", \"5\", \"6\", \"7\", \"8\", \"9\", \"COMMA\", \"DOT\"],\n";
	output << "\"posx\": \"0.75\",\n";
	output << "\"posy\": \"-0.78\",\n";
	output << "\"posz\": \"0.0\",\n";
	output << "\"scale\": \"0.05\",\n";
	output << "\"exsist\": \"true\",\n";
	output << "\"optionalProperties\": \"LABEL_THREE\"\n";
	output << "}\n";
	output << ",\n";
	output << "{\n";
	output << "\"object_type\": \"BUTTON\",\n";
	output << "\"models_type\": [\"MENU_BUTTON\", \"MENU_BUTTON\"],\n";
	output << "\"posx\": \"0.0\",\n";
	output << "\"posy\": \"-0.85\",\n";
	output << "\"posz\": \"0.0\",\n";
	output << "\"scale\": \"0.1\",\n";
	output << "\"exsist\": \"true\",\n";
	output << "\"optionalProperties\": \"{ 'func' : 'MENU_BUTTON', 'visible' : 'false' }\"\n";
	output << "}\n";
	output << ",\n";
	output << "{\n";
	output << "\"object_type\": \"2D_LINE\",\n";
	output << "\"models_type\": [\"2D_LINE\"],\n";
	output << "\"posx\": \"0.0\",\n";
	output << "\"posy\": \"0.0\",\n";
	output << "\"posz\": \"0.0\",\n";
	output << "\"scale\": \"0.0\",\n";
	output << "\"exsist\": \"false\",\n";
	output << "\"optionalProperties\": \"\"\n";
	output << "}\n";
	output << "]\n";

	output.flush();
	output.close();
}
