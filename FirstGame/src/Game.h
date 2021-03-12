#pragma once
#include <iostream>
#include <map>
#include <memory>
#include <thread>
#include <future>

#include "RenderEngine.h"
#include "Unit.h"
#include "Planet.h"
#include "WhiteStar.h"
#include "SelectArea.h"
#include "Button.h"
#include "Utilities.h"
#include "Label.h"
#include "Statistic.h"

class Planet;

class Game
{
public:
	Game(uint32_t level);
	~Game();

	//Game(Game&) = delete;				//??
	//Game(Game&&) = delete;			//??
	Game& operator==(Game&) = delete;	//??

	void loop();
	void createObject(ObjectAttributes& attributes);
	void createUnit(const std::string& unitType, const glm::vec3& position, const glm::vec3& targetPosition, const std::string& fraction);
	void updateGameState(float deltaTime);
	void processInput(bool* keys, bool* buttons, const glm::vec2& cursorCoords, double scroll);
	void updatePhysics();
	void collisionsDetected();
	bool checkButtonHits(const glm::vec2& cursorCoords, bool isPressed);
	void setTargetForSelectedUnits(const glm::vec2& cursorCoords); 
	bool checkEndGame();
	void drawStatisticScreen();
	void generateWhiteStars();
	void switchVisibleLabelsAndButtonsForStatScreen();
	void createGame(uint32_t& level, uint32_t& progress);
	void loadResources(uint32_t level);
	void freeResources();
	void artificialInteligence();
	//void setTarget(std::vector<std::shared_ptr<Object>>& units, glm::vec3& targetPos, glm::vec3& planetPos, std::string& planetFraction, float planetLevel, float planetLevelMAX);
	void setTarget(std::vector<std::shared_ptr<Object>>& units, glm::vec3& targetPos, Planet* targetPlanet);

	//callback functions (for buttons)
	friend void switchPause(Game& game);
	friend void speedGameUp(Game& game);
	friend void speedGameDown(Game& game);
	friend void startButton(Game& game);
	friend void toMenuButton(Game& game);
	friend void startMapEditor(Game& game);
	friend void processArea(const bool& leftButton, const glm::vec2& cursorCoords, Game& game);
	friend void selectUnits(Game& game);
	friend void selectPlanets(Game& game);
	friend void deletePlanets(Game& game);
	friend void createRedPlanet(Game& game);
	friend void createBluePlanet(Game& game);
	friend void createNeutralPlanet(Game& game);
	friend void saveMap(Game& game);

	struct Edges
	{
		float left;
		float right;
		float top;
		float bot;

		Edges(float l, float r, float t, float b) : left(l), right(r), top(t), bot(b)
			{}
	};
	std::shared_ptr<Edges> m_edgesOfMap;

private:
	bool m_run;
	bool m_pause;
	bool m_inMenu;
	bool m_inMapEditor;
	bool m_generateStar;
	float m_lastTime;

	//for map editor
	Object* m_inHand;
	void(*processLeftButton)(const bool& leftButton, const glm::vec2& cursorCoords, Game& game);
	void(*selectOblects)(Game& game);

	std::string m_player_fraction;
	std::string m_ai_fraction;
	std::string m_winner;

	std::shared_ptr<Statistic> m_statistic;

	std::vector<std::shared_ptr<Object>> m_objects;
	std::vector<std::shared_ptr<Object>> m_red_units;
	std::vector<std::shared_ptr<Object>> m_blue_units;
	std::vector<std::shared_ptr<Label>> m_labels;
	std::shared_ptr<select_area> m_area;
};