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
	glm::vec2 startCursorPos;
	bool firstMouse;
	int scroll;

	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;
	std::shared_ptr<Camera> camera;

	float deltaTime;

	ResourceManager resourceManager;

	std::vector<modelGroupAttribs> modelsGroups;
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

	glfwSwapInterval(1);

	//
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
	float offset = (camera->finalCameraPoint - camera->Position.y) * 0.07f;
	camera->Position.y += offset;

	//camera->Position.y = camera->finalCameraPoint;

	view = camera->GetViewMatrix();
	projection = glm::perspective(glm::radians(camera->Zoom), (float)SCREEN.x / (float)SCREEN.y, 0.1f, 1000.0f);
}

void RenderEngine::updateScreen()
{
	glfwSwapBuffers(window);
}


void RenderEngine::genModelMatrices(std::vector<std::shared_ptr<Object>>& objects)
{
	for (auto&& group : modelsGroups)
		group.matrices.clear();

	for (auto&& obj : objects)
	{
		for (auto&& index : obj->m_indexes_of_displayd_models)
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, obj->m_position);
			model = glm::scale(model, glm::vec3(obj->m_scale, obj->m_scale, obj->m_scale));

			modelsGroups[obj->m_modelIDs[index]].matrices.push_back(model);
		}
	}
}

void RenderEngine::drawObjects()
{
	for (auto&& group : modelsGroups)
	{
		if (group.depth)
			glEnable(GL_DEPTH_TEST);
		else
			glDisable(GL_DEPTH_TEST);

		if (group.matrices.size() > 30)
			drawGroupOfObjects(group);
		else
			drawSingleObject(group);
	}

	//draw area
	//-----------------------------
	for (unsigned int modelID = 0; modelID < RenderEngine::resourceManager.m_manCrObj_indexs.size(); modelID++)
	{
		uint32_t modelIndex = RenderEngine::resourceManager.m_manCrObj_indexs[modelID].first;
		uint32_t shaderIndex = RenderEngine::resourceManager.m_manCrObj_indexs[modelID].second;

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, RenderEngine::resourceManager.m_manuallyCreaatedObjects[modelIndex].m_position);

		resourceManager.shaders[shaderIndex].second.use();
		resourceManager.shaders[shaderIndex].second.setMat4("view", view);
		resourceManager.shaders[shaderIndex].second.setMat4("projection", projection);
		resourceManager.shaders[shaderIndex].second.setMat4("model", model);

		glDisable(GL_DEPTH_TEST);
		glBindVertexArray(RenderEngine::resourceManager.m_manuallyCreaatedObjects[modelIndex].m_VAO);
		glLineWidth(3);
		glDrawElements(GL_LINE_LOOP, RenderEngine::resourceManager.m_manuallyCreaatedObjects[modelIndex].m_vertices.size() / 3, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		glEnable(GL_DEPTH_TEST);
	}
}

void RenderEngine::drawGroupOfObjects(RenderEngine::modelGroupAttribs& group)
{
	if (!group.bufferIsCreated)
	{
		glGenBuffers(1, &group.buffer);
		group.bufferIsCreated = true;
	}

	glBindBuffer(GL_ARRAY_BUFFER, group.buffer);
	glBufferData(GL_ARRAY_BUFFER, group.matrices.size() * sizeof(glm::mat4), &group.matrices[0], GL_STATIC_DRAW);


	for (unsigned int i = 0; i < std::get<1>(resourceManager.models[group.model_index]).meshes.size(); i++)
	{
		unsigned int VAO = std::get<1>(resourceManager.models[group.model_index]).meshes[i].VAO;
		glBindVertexArray(VAO);
		// set attribute pointers for matrix (4 times vec4)
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);

		glBindVertexArray(0);
	}


	//it's draw groups function
	//by default INSTANSING shader has index - 1
	resourceManager.shaders[group.shaders_indices[1]].second.use();
	resourceManager.shaders[group.shaders_indices[1]].second.setMat4("projection", projection);
	resourceManager.shaders[group.shaders_indices[1]].second.setMat4("view", view);


	if (!std::get<1>(resourceManager.models[group.model_index]).textures_loaded.empty())
	{
		resourceManager.shaders[group.shaders_indices[1]].second.setInt("texture_diffuse1", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, std::get<1>(resourceManager.models[group.model_index]).textures_loaded[0].id);
	}

	for (unsigned int i = 0; i < std::get<1>(resourceManager.models[group.model_index]).meshes.size(); i++)
	{
		glBindVertexArray(std::get<1>(resourceManager.models[group.model_index]).meshes[i].VAO);
		glDrawElementsInstanced(GL_TRIANGLES, std::get<1>(resourceManager.models[group.model_index]).meshes[i].indices.size(), GL_UNSIGNED_INT, 0, group.matrices.size());
		glBindVertexArray(0);
	}
}

void RenderEngine::drawSingleObject(RenderEngine::modelGroupAttribs& group)
{
	for (auto&& matrix : group.matrices)
	{
		//it's draw single object function
		//by default BASIC shader has index - 0
		resourceManager.shaders[group.shaders_indices[0]].second.use();

		resourceManager.shaders[group.shaders_indices[0]].second.setMat4("projection", projection);
		resourceManager.shaders[group.shaders_indices[0]].second.setMat4("view", view);
		resourceManager.shaders[group.shaders_indices[0]].second.setMat4("model", matrix);

		std::get<1>(resourceManager.models[group.model_index]).Draw(resourceManager.shaders[group.shaders_indices[0]].second);
	}
}

void RenderEngine::loadLevel(unsigned int number, std::vector<ObjectAttributes>& objectsAttrib)
{
	resourceManager.loadModels(number, objectsAttrib);

	// create models groups
	for (auto&& x : resourceManager.modelIndex_shadersIndices)
		modelsGroups.push_back({ std::get<2>(resourceManager.models[x.first]).depth_test, false, 0, x.first, x.second, std::vector<glm::mat4>() });
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

float RenderEngine::getScreenAspectRatio()
{
	return SCREEN.y / SCREEN.x;
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