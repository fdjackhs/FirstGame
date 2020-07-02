// OpenGL implementation
#include "RenderEngine.h"

#include <GLFW/glfw3.h>

//callback functions
//------------------
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void cursor_enter_callback(GLFWwindow* window, int entered);
//------------------


namespace RenderEngine
{
	glm::vec2 SCREEN;
	GLFWwindow* window;

	//keyboard
	bool keys[349];
	//mouse
	bool enteredWindow;
	bool buttons[12];
	glm::vec2 cursorCoords;
	glm::vec3 startCursorPos;
	bool firstMouse;
	int scroll;

	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;
	std::shared_ptr<Camera> camera;

	float deltaTime;

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
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetCursorEnterCallback(window, cursor_enter_callback);
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
	glDepthFunc(GL_LESS);
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	//
	camera = std::make_shared<Camera>(glm::vec3(0.0f, 10.0f, 0.0f));
	RenderEngine::firstMouse = true;
	srand(unsigned int(time(NULL)));

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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
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


void RenderEngine::drawObjects(std::vector<std::shared_ptr<Object>>& objects, select_area& selectArea)
{
	//draw objects
	//------------
	for (auto&& obj : objects)
	{
		for (unsigned int modelID = 0; modelID < obj->m_modelIDs.size(); modelID++)
		{
			if (obj->m_selected)
			{
				glStencilFunc(GL_ALWAYS, 1, 0xFF);
				glStencilMask(0xFF);
			}

				//draw original objects
				//---------------------
				resourceManager.shaders[resourceManager.modelIndex_shaderIndex[modelID].second].second.use();
				resourceManager.shaders[resourceManager.modelIndex_shaderIndex[modelID].second].second.setMat4("projection", projection);
				resourceManager.shaders[resourceManager.modelIndex_shaderIndex[modelID].second].second.setMat4("view", view);
				glm::vec3 position = obj->GetPosition();

				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model, { position.x, position.y, position.z });
				model = glm::scale(model, glm::vec3(obj->m_scale, obj->m_scale, obj->m_scale));

				resourceManager.shaders[resourceManager.modelIndex_shaderIndex[modelID].second].second.setMat4("model", model);
				resourceManager.models[resourceManager.modelIndex_shaderIndex[modelID].first].second.Draw(resourceManager.shaders[resourceManager.modelIndex_shaderIndex[modelID].second].second);
				//---------------------

			//draw stencil
			//------------
			if (obj->m_selected)
			{
				glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
				glStencilMask(0x00);

				glDisable(GL_DEPTH_TEST);
				resourceManager.m_stencilShader->use();
				resourceManager.m_stencilShader->setMat4("projection", projection);
				resourceManager.m_stencilShader->setMat4("view", view);

				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model, { position.x, position.y, position.z });
				model = glm::scale(model, glm::vec3(obj->m_scale * 1.7f, obj->m_scale * 1.7f, obj->m_scale * 1.7f));

				resourceManager.m_stencilShader->setMat4("model", model);
				resourceManager.models[resourceManager.modelIndex_shaderIndex[modelID].first].second.Draw(*resourceManager.m_stencilShader);

				glStencilMask(0xFF);
				glStencilFunc(GL_ALWAYS, 0, 0xFF);
				glEnable(GL_DEPTH_TEST);
			}
			//------------
		}
	}

	//draw area
	//-----------------------------
	
	if (selectArea.radius > 0.0f)
	{
		selectArea.updateVertices();
		resourceManager.updateVBO(selectArea.vbo, selectArea.vertices);

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, selectArea.centerInWorldCoords);

		RenderEngine::resourceManager.m_areaShader->use();
		for (auto&& object : RenderEngine::resourceManager.m_manuallyCreaatedObjects)
		{

			RenderEngine::resourceManager.m_areaShader->setMat4("view", view);
			RenderEngine::resourceManager.m_areaShader->setMat4("projection", projection);
			RenderEngine::resourceManager.m_areaShader->setMat4("model", model);

			glBindVertexArray(selectArea.vao);
			glLineWidth(3);
			glDrawElements(GL_LINE_LOOP, selectArea.size / 3, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}
	}
}



glm::vec3 RenderEngine::cursorCoordToWorldCoords(const glm::vec2& cursorPos)
{
	//I promise that someday I will find out how this magic works.

	glm::vec4 tmp((2.0 * cursorPos.x / RenderEngine::SCREEN.x - 1.0f), (-2.0 * cursorPos.y / RenderEngine::SCREEN.y + 1.0f), -1.0f, 1.0f);
	glm::vec4 iTmp = glm::vec4(glm::vec2(glm::inverse(RenderEngine::projection) * tmp), -1.0f, 0.0f);
	glm::vec3 direction(glm::normalize(glm::vec3(glm::inverse(RenderEngine::camera->GetViewMatrix()) * iTmp)));
	glm::vec3 camPos(glm::vec3(glm::inverse(RenderEngine::camera->GetViewMatrix()) * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)));
	glm::vec3 N(0.0f, 1.0f, 0.0f);
	float t = (-glm::dot(camPos, N)) / (glm::dot(direction, N));
	glm::vec3 result = camPos + direction * t;

	return result;
}

glm::vec2 RenderEngine::WorldCoordsToScreenCoords(const glm::vec3& objPos)
{
	glm::vec4 ScreenCoords = projection * view * model * glm::vec4(objPos, 1.0);
	glm::vec2 normalizedCoords = { (ScreenCoords.x + 10) / 20 , (ScreenCoords.y + 10) / 20 };
	glm::vec2 result = { normalizedCoords.x * SCREEN.x , normalizedCoords.y * SCREEN.y };
	return result;
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

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	RenderEngine::cursorCoords.x = (float)xpos;
	RenderEngine::cursorCoords.y = (float)ypos;

	//std::cout << "coursorCoords.x " << RenderEngine::coursorCoords.x << " coursorCoords.y " << RenderEngine::coursorCoords.y << std::endl;
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
	if (action == GLFW_PRESS)
		RenderEngine::buttons[button] = true;
	if (action == GLFW_RELEASE)
		RenderEngine::buttons[button] = false;

	//std::cout << "button " << button << " action " << action << std::endl;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	RenderEngine::scroll = (int)yoffset;
	//std::cout << "xoffset " << xoffset << " yoffset " << yoffset << std::endl;
}

void cursor_enter_callback(GLFWwindow* window, int entered)
{
	if (entered)
		RenderEngine::enteredWindow = true;
	else
		RenderEngine::enteredWindow = false;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
//------------------