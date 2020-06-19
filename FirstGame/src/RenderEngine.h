#pragma once

namespace RenderEngine
{
	int init(unsigned int x, unsigned int y, const char* windowName);
	
	bool windowShouldClose();

	void clearScreen();

	void updateScreen();

	void pollEvents();

	void free();
}