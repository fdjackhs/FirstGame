#include "Game.h"


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
			for (auto&& ch : attributes.optionalProperties) { if (ch == '\'') ch = '"'; };

			rapidjson::Document d;
			d.Parse(attributes.optionalProperties.c_str());

			rapidjson::Value& func = d["func"];
			rapidjson::Value& vis = d["visible"];

			std::string function = func.GetString();
			std::string visible = vis.GetString();


			Button* ptr_button = new Button(RenderEngine::resourceManager.m_complete_models[attributes.id],
				glm::vec3{ stof(attributes.posx), stof(attributes.posy), stof(attributes.posz) },
				stof(attributes.scale),
				this,
				function);

			ptr_button->m_visible = visible == "true";
			std::shared_ptr<Button> sh_button(ptr_button);

			if (function == "PAUSE")
				sh_button->setCallbackFunc(switchPause);
			if (function == "SPEED_UP")
				sh_button->setCallbackFunc(speedGameUp);
			if (function == "SPEED_DOWN")
				sh_button->setCallbackFunc(speedGameDown);
			if (function == "BUTTON_START")
				sh_button->setCallbackFunc(startButton);
			if (function == "BUTTON_CREATE_MAP")
				sh_button->setCallbackFunc(startMapEditor);
			if (function == "MENU_BUTTON")
				sh_button->setCallbackFunc(toMenuButton);
			if (function == "DELETE")
				sh_button->setCallbackFunc(deletePlanets);
			if (function == "CREATE_RED_PLANET")
				sh_button->setCallbackFunc(createRedPlanet);
			if (function == "CREATE_BLUE_PLANET")
				sh_button->setCallbackFunc(createBluePlanet);
			if (function == "CREATE_NEUTRAL_PLANET")
				sh_button->setCallbackFunc(createNeutralPlanet);
			if (function == "SAVE")
				sh_button->setCallbackFunc(saveMap);

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
			if (attributes.optionalProperties == "LABEL_TWO" ||
				attributes.optionalProperties == "LABEL_THREE" ||
				attributes.optionalProperties == "PAUSE" ||
				attributes.optionalProperties == "RED_WON" ||
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
			uint32_t id = RenderEngine::resourceManager.createObject(std::vector<float>(240), "./FirstGame/Resources/shaders/1.area_shader.vs", "./FirstGame/Resources/shaders/1.area_shader.fs");
			m_area = std::make_shared<select_area>(id);
		}
		else if (attributes.object_type == "EDGES")
		{
			for (auto&& ch : attributes.optionalProperties) { if (ch == '\'') ch = '"'; };

			rapidjson::Document d;
			d.Parse(attributes.optionalProperties.c_str());

			rapidjson::Value& left = d["left"];
			rapidjson::Value& right = d["right"];
			rapidjson::Value& top = d["top"];
			rapidjson::Value& bot = d["bot"];

			//Edges* edges = new Edges();
			m_edgesOfMap = std::make_shared<Edges>(left.GetFloat(), right.GetFloat(), top.GetFloat(), bot.GetFloat());
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
