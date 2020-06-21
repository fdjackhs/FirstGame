// OpenGL implementation
#include "RenderEngine.h"
/*
extern glm::vec2 SCREEN;
extern GLFWwindow* window;
extern bool keys[1024];

extern std::shared_ptr<Camera> camera;

extern glm::mat4 model;
extern glm::mat4 view;
extern glm::mat4 projection;
*/

//callback functions
//------------------
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
//------------------

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


	//glViewport(0, 0, width, height);
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGL())
	{
		std::cout << "gladLoadGL() failed" << std::endl;
		glfwTerminate();
		return -1;
	}

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
	glClear(GL_COLOR_BUFFER_BIT);
}

void RenderEngine::updateCameraView()
{
	model = glm::mat4(1.0f);
	view = camera->GetViewMatrix();
	projection = glm::perspective(glm::radians(camera->Zoom), (float)SCREEN.x / (float)SCREEN.y, 0.1f, 100.0f);
}

void RenderEngine::updateScreen()
{
	glfwSwapBuffers(window);
}


void RenderEngine::drawObjects(const std::vector<Object>& objects)
{

}

void RenderEngine::pollEvents()
{
	glfwPollEvents();
}

void RenderEngine::clearKeys()
{
	for (auto&& x : keys)
		x = false;
}


//callback functions
//------------------
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	std::cout << key << std::endl;

	if (action == GLFW_PRESS || action == GLFW_RELEASE)
		RenderEngine::keys[key] = true;

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