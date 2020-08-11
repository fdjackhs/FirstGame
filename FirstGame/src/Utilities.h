#pragma once
#include <string>

#include <glm/glm.hpp>

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
	}
};
