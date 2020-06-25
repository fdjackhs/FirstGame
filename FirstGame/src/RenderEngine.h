#pragma once
#include <vector>
#include <iostream>
#include <map>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>

#include "Object.h"
#include "Camera.h"
#include "Shader.h"
#include "ResourceManager.h"

namespace RenderEngine
{
	extern glm::vec2 SCREEN;
	extern GLFWwindow* window;

	//keyboard
	extern bool keys[349];
	//mouse
	extern bool enteredWindow;
	extern bool buttons[12];
	extern glm::vec2 cursorCoords;
	extern glm::vec3 startCursorPos;
	extern bool firstMouse;
	extern int scroll;
	
	extern glm::mat4 model;
	extern glm::mat4 view;
	extern glm::mat4 projection;
	extern std::shared_ptr<Camera> camera;
	
	extern double deltaTime;
	
	extern ResourceManager resourceManager;

	//Render routines
	//----------------------------------------------

	int init(float x, float y, const char* windowName);
	void free();

	float getCurrTime();

	bool windowShouldClose();
	void clearScreen();
	void updateCameraView();
	void updateScreen();

	glm::vec3 cursorCoordToWorldCoords(const glm::vec2& cursorPos);

	void drawObjects(const std::vector<Object>& objects);

	void pollEvents();
};