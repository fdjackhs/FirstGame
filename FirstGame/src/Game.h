#pragma once
#include <iostream>
#include <map>
#include <memory>

#include "RenderEngine.h"
#include "Unit.h"
#include "Planet.h"
#include "SelectArea.h"
#include "Button.h"
#include "CustomDistance.h"

class Game
{
public:
	Game(unsigned int level);
	~Game();

	Game(Game&) = delete;				//??
	Game(Game&&) = delete;				//??
	Game& operator==(Game&) = delete;	//??

	void loop();
	void createObject(const ObjectAttributes& attributes);
	void createUnit(const std::string& unitType, const glm::vec3& position, const glm::vec3& targetPosition, const std::string& fraction);
	void updateGameState(float deltaTime);
	void processInput(bool* keys, bool* buttons, const glm::vec2& cursorCoords, double scroll);
	void selectUnits();
	void updatePhysics();
	void collisionsDetected();
	bool checkButtonHits(const glm::vec2& cursorCoords, bool isPressed);
	void setTargetForSelectedUnits(const glm::vec2& cursorCoords);


	//callback functions (for buttons)
	friend void switchPause(Game& game);
	friend void speedGameUp(Game& game);
	friend void speedGameDown(Game& game);

private:
	bool m_pause;
	float m_timeKoef;

	std::vector<std::shared_ptr<Object>> objects;

	std::vector<std::shared_ptr<Unit>> red_units;
	std::vector<std::shared_ptr<Unit>> blue_units;

	std::shared_ptr<select_area> m_area;

	std::string m_player_fraction;
	std::string m_ai_fraction;
};