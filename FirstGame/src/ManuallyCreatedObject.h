#pragma once
#include <string>
#include <vector>

#include <glfw/glfw3.h>

#include "glm/vec2.hpp"
#include "glm/vec3.hpp"

class ManuallyCreatedObject
{
public:
	ManuallyCreatedObject(GLuint& vao, GLuint& vbo, GLuint& ebo, 
						  float scale, glm::vec3& position,
						  std::vector<GLfloat>& m_vertices);

	//~ManuallyCreatedObject();

	GLuint m_VAO, m_VBO, m_EBO;

	float m_scale;
	glm::vec3 m_position;
	std::vector<GLfloat> m_vertices;
	// prams for drawing
};