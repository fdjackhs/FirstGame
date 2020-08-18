#pragma once
#include <string>

#include <glm/glm.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Shader.h"
#include "Camera.h"

static bool closerThan(const glm::vec3& firstPosition, const glm::vec3& secondPosition, const float& distance)
{
	float x = firstPosition.x - secondPosition.x;
	//float y = firstPosition.y - secondPosition.y;
	float z = firstPosition.z - secondPosition.z;

	//return (x * x + y * y + z * z) < distance * distance;
	return (x * x + z * z) < distance * distance;
}

static float sqrt_distance(const glm::vec3& firstPosition, const glm::vec3& secondPosition)
{
	float x = firstPosition.x - secondPosition.x;
	float z = firstPosition.z - secondPosition.z;
	return x * x + z * z;
}

class Var
{
	void* m_value;
	std::string m_type;

public:

	Var() 
	{
		m_value = nullptr;
		m_type = "";
	}

	Var(const std::string& type, void* value) : m_value(value), m_type(type)
	  {}

	std::string toString()
	{
		std::string result = "";
		if (m_type == "string")
		{
			result = *(static_cast<std::string*>(m_value));
			return result;
		}
		else if (m_type == "int32_t")
		{
			result = std::to_string(*(static_cast<int32_t*>(m_value)));
			return result;
		}
		else if (m_type == "uint32_t")
		{
			result = std::to_string(*(static_cast<uint32_t*>(m_value)));
			return result;
		}
		else if (m_type == "float")
		{
			result = std::to_string(*(static_cast<float*>(m_value)));
			return result;
		}

		return "";
	}
};

static void draw3Dline(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& color, std::vector<std::pair<std::string, Shader>>& shaders, Camera& cam)
{
	GLfloat vertices[] = {
		 p1.x, p1.y, p1.z,
		 p1.x + p2.x, p1.y + p2.y, p1.z + p2.z
	};
	
	GLuint indices[] = { 0, 1 };

	static GLuint VBO, VAO, EBO;
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &EBO);

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


	int32_t shaderIndex = -1;
	for (uint32_t i = 0; i < shaders.size(); i++)
	{
		if (shaders[i].first == "AREA")
			shaderIndex = i;
	}

	shaders[shaderIndex].second.use();
	shaders[shaderIndex].second.setVec3("color", color);

	glm::mat4 view = cam.GetViewMatrix();
	glm::mat4 projection = glm::perspective(glm::radians(cam.Zoom), (float)1600.0f / (float)800.0f, 0.1f, 1000.0f);

	shaders[shaderIndex].second.setMat4("view", view);
	shaders[shaderIndex].second.setMat4("projection", projection);

	glm::mat4 model = glm::mat4(1.0f);
	shaders[shaderIndex].second.setMat4("model", model);

	glBindVertexArray(VAO);
	glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

static void draw2Dline(const glm::vec2& p1, const glm::vec2& p2, const glm::vec3& color, std::vector<std::pair<std::string, Shader>>& shaders, Camera& cam)
{
	GLfloat vertices[] = {
		 p1.x, p1.y, 0.0f,
		 p2.x, p2.y, 0.0f
	};

	GLuint indices[] = { 0, 1 };

	static GLuint VBO, VAO, EBO;
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &EBO);

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


	int32_t shaderIndex = -1;
	for (uint32_t i = 0; i < shaders.size(); i++)
	{
		if (shaders[i].first == "2D_LINE")
			shaderIndex = i;
	}

	shaders[shaderIndex].second.use();
	shaders[shaderIndex].second.setVec3("color", color);

	glm::mat4 model = glm::mat4(1.0f);
	shaders[shaderIndex].second.setMat4("model", model);

	glBindVertexArray(VAO);
	glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
