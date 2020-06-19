// OpenGL implementation
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>

#include "RenderEngine.h"

namespace RenderEngine
{
	glm::vec2 SCREEN;
	GLFWwindow* window = nullptr;

	bool keys[1024];
	//TODO: mouse pos & mouse buttons & scroll

	//callback function difinitions
	//-----------------------------
	void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
	void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	//-----------------------------

	int init(unsigned int x, unsigned int y, const char* windowName)
	{
		SCREEN.x = x;
		SCREEN.y = y;

		if (!glfwInit())
		{
			std::cout << "glfwInit() failed" << std::endl;
			return -1;
		}
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		window = glfwCreateWindow(SCREEN.x, SCREEN.y, windowName, nullptr, nullptr);
		if (!window)
		{
			std::cout << "glfwCreateWindow failed" << std::endl;
			glfwTerminate();
			return -1;
		}
		glfwMakeContextCurrent(window);
		glfwSetKeyCallback(window, key_callback);
		glfwSetCursorPosCallback(window, mouse_callback);
		glfwSetScrollCallback(window, scroll_callback);
		glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


		//glViewport(0, 0, width, height);
		//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		if (!gladLoadGL())
		{
			std::cout << "gladLoadGL() failed" << std::endl;
			glfwTerminate();
			return -1;
		}

		return 0;
	}

	void drawObjects(const std::vector<Object>& objects)
	{

	}

	bool windowShouldClose()
	{
		return glfwWindowShouldClose(RenderEngine::window);
	}

	void clearScreen()
	{
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void updateScreen()
	{
		glfwSwapBuffers(window);
	}

	void pollEvents()
	{
		glfwPollEvents();
	}
		
	void free()
	{
		glfwTerminate();
	}

	//callback functions
	//------------------
	void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		std::cout << key << std::endl;

		if (action == GLFW_PRESS || action == GLFW_RELEASE)
			keys[key] = true;

		//dont forget clear keys
	}
	void framebuffer_size_callback(GLFWwindow* window, int width, int height)
	{
		glViewport(0, 0, width, height);
	}
	void mouse_callback(GLFWwindow* window, double xpos, double ypos)
	{
		std::cout << "xpos " << xpos << " ypos " << ypos << std::endl;
	}
	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
	{
		std::cout << "xoffset " << xoffset << " yoffset " << yoffset << std::endl;
	}
	//------------------
}