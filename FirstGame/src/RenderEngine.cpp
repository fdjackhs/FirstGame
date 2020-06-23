// OpenGL implementation
#include "RenderEngine.h"

//callback functions
//------------------
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
//------------------


namespace RenderEngine
{
	glm::vec2 SCREEN;
	GLFWwindow* window;
	bool keys[1024];

	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;
	std::shared_ptr<Camera> camera;

	double deltaTime;

	ResourceManager resourceManager;
}

int RenderEngine::init(float x, float y, const char* windowName)
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

	window = glfwCreateWindow((int)SCREEN.x, (int)SCREEN.y, windowName, nullptr, nullptr);
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


	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGL())
	{
		std::cout << "gladLoadGL() failed" << std::endl;
		glfwTerminate();
		return -1;
	}

	glViewport(0, 0, (int)SCREEN.x, (int)SCREEN.y);

	glEnable(GL_DEPTH_TEST);

	camera = std::make_shared<Camera>(glm::vec3(0.0f, 0.0f, 3.0f));

	return 0;
}

void RenderEngine::free()
{
	glfwTerminate();
}

float RenderEngine::getCurrTime()
{
	return (float)glfwGetTime();
}

bool RenderEngine::windowShouldClose()
{
	return glfwWindowShouldClose(RenderEngine::window);
}

void RenderEngine::clearScreen()
{
	glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RenderEngine::updateCameraView()
{
	view = camera->GetViewMatrix();
	projection = glm::perspective(glm::radians(camera->Zoom), (float)SCREEN.x / (float)SCREEN.y, 0.1f, 100.0f);
}

void RenderEngine::updateScreen()
{
	glfwSwapBuffers(window);
}


void RenderEngine::drawObjects(const std::vector<Object>& objects)
{

	for (auto&& x : objects)
	{
		resourceManager.shaders[resourceManager.modelIndex_shaderIndex[x.modelID].second].second.use();

		resourceManager.shaders[resourceManager.modelIndex_shaderIndex[x.modelID].second].second.setMat4("projection", projection);
		resourceManager.shaders[resourceManager.modelIndex_shaderIndex[x.modelID].second].second.setMat4("view", view);

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, x.position);
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));

		//glUseProgram(resourceManager.shaders[resourceManager.modelIndex_shaderIndex[x.modelID].second].second.ID);

		//int vertexColorLocation = glGetUniformLocation(resourceManager.shaders[resourceManager.modelIndex_shaderIndex[x.modelID].second].second.ID, "ourColor");
		//glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);

		//resourceManager.shaders[resourceManager.modelIndex_shaderIndex[x.modelID].second].second.setVec4("ourColor", { 0.0f, greenValue, 0.0f, 1.0f });

		//glDrawArrays(GL_TRIANGLES, 0, 3);



		resourceManager.shaders[resourceManager.modelIndex_shaderIndex[x.modelID].second].second.setMat4("model", model);

		resourceManager.models[resourceManager.modelIndex_shaderIndex[x.modelID].first].second.Draw(resourceManager.shaders[resourceManager.modelIndex_shaderIndex[x.modelID].second].second);
	}
}

void RenderEngine::pollEvents()
{
	glfwPollEvents();
}

//callback functions
//------------------
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		//std::cout << key << " " << action << std::endl;
		RenderEngine::keys[key] = true;
	}
	if (action == GLFW_RELEASE)
		RenderEngine::keys[key] = false;
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	//std::cout << "xpos " << xpos << " ypos " << ypos << std::endl;
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	//std::cout << "xoffset " << xoffset << " yoffset " << yoffset << std::endl;
}
//------------------