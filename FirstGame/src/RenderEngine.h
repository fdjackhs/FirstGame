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
	extern bool keys[1024];
	
	extern glm::mat4 model;
	extern glm::mat4 view;
	extern glm::mat4 projection;
	extern std::shared_ptr<Camera> camera;
	
	extern double deltaTime;
	
	extern ResourceManager resourceManager;

	//Render routines
	//----------------------------------------------
	//TODO: mouse pos & mouse buttons & scroll

	int init(float x, float y, const char* windowName);
	void free();

	float getCurrTime();

	bool windowShouldClose();
	void clearScreen();
	void updateCameraView();
	void updateScreen();

	void drawObjects(const std::vector<Object>& objects);

	void pollEvents();
};