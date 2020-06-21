#pragma once
#include <vector>
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>


#include "Object.h"
#include "Camera.h"
#include "Shader.h"

namespace RenderEngine
{
//public:
	//RenderEngine() = delete;

	static glm::vec2 SCREEN;
	static GLFWwindow* window;
	static bool keys[1024];

	static glm::mat4 model;
	static glm::mat4 view;
	static glm::mat4 projection;
	static std::shared_ptr<Camera> camera;

	static double deltaTime;

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
	void clearKeys();
};