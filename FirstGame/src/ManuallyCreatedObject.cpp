#include "ManuallyCreatedObject.h"

ManuallyCreatedObject::ManuallyCreatedObject(GLuint& vao, GLuint& vbo, GLuint& ebo,
											 float scale, glm::vec3& position,
											 std::vector<GLfloat>& vertices)
{
	m_VAO = vao;
	m_VBO = vbo;
	m_EBO = ebo;

	m_scale = scale;
	m_position = position;

	m_vertices = move(vertices); //?
}