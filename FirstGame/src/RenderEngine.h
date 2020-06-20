#pragma once

#include <vector>

#include "Object.h"

namespace RenderEngine
{
	//TODO: mouse pos & mouse buttons & scroll

	int init(unsigned int x, unsigned int y, const char* windowName);
	
	void drawObjects(const std::vector<Object>& objects);

	bool windowShouldClose();

	void clearScreen();

	void updateScreen();

	void pollEvents();

	void free();
}