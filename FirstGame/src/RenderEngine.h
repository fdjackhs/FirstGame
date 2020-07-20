#pragma once
#include <vector>
#include <iostream>
#include <map>
#include <math.h>
#include <time.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>

#include "Object.h"
#include "Camera.h"
#include "Shader.h"
#include "ResourceManager.h"
#include "SelectArea.h"

extern class select_area;

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
	
	extern float deltaTime;
	
	extern ResourceManager resourceManager;

	struct modelGroupAttribs
	{
		bool depth;
		bool bufferIsCreated;
		uint32_t buffer;

		uint32_t model_index;
		std::vector<uint32_t> shaders_indices;

		std::vector<glm::mat4> matrices;
	};
	extern std::vector<modelGroupAttribs> modelsGroups;

	//Render routines
	//----------------------------------------------

	int init(float x, float y, const char* windowName);
	void free();

	float getCurrTime();

	bool windowShouldClose();
	void clearScreen();
	void updateCameraView();
	void updateScreen();
	void pollEvents();

	glm::vec3 cursorCoordToWorldCoords(const glm::vec2& cursorPos);
	glm::vec2 WorldCoordsToScreenCoords(const glm::vec3& objPos);

	void drawObjects(std::vector<std::shared_ptr<Object>>& objects);
	void genModelMatrices(std::vector<std::shared_ptr<Object>>& objects);
	void drawGroupOfObjects(RenderEngine::modelGroupAttribs& group);
	void drawSingleObject(RenderEngine::modelGroupAttribs& group);
};