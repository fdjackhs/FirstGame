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
	void selectUnits();
	void updatePhysics();
	void collisionsDetected();
	bool checkButtonHits(const glm::vec2& cursorCoords, bool isPressed);
	void setTargetForSelectedUnits(const glm::vec2& cursorCoords); 
	void processArea(const bool& leftButton, const glm::vec2& cursorCoords);
	bool checkEndGame();
	void drawStatisticScreen();
	void generateWhiteStars();
	void switchVisibleLabelsAndButtonsForStatScreen();
	void createGame(uint32_t& level, uint32_t& progress);
	void loadResources(uint32_t level);
	void freeResources();

	//callback functions (for buttons)
	friend void switchPause(Game& game);
	friend void speedGameUp(Game& game);
	friend void speedGameDown(Game& game);
	friend void startButton(Game& game);
	friend void toMenuButton(Game& game);
	friend void startMapEditor(Game& game);


	struct Edges
	{
		float left;
		float right;
		float top;
		float bot;
	} m_edgesOfMap;

private:
	bool m_run;
	bool m_pause;
	bool m_inMenu;
	float m_lastTime;

	bool m_generateStar;

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