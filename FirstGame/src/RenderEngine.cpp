// OpenGL implementation

#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>

namespace RenderEngine
{
	glm::vec2 SCREEN;
	GLFWwindow* window = nullptr;

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

		if (!gladLoadGL())
		{
			std::cout << "gladLoadGL() failed" << std::endl;
			glfwTerminate();
			return -1;
		}

		return 0;
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
}