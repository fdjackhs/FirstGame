#pragma once

#include "RenderEngine.h"

class Game
{
public:

	void loop();
	void load();
	void updateGameState();
	void poolEvents();
};