#pragma once
#include <glm/glm.hpp>

static bool closerThan(const glm::vec3& firstPosition, const glm::vec3& secondPosition, const float& distance)
{
	float x = firstPosition.x - secondPosition.x;
	//float y = firstPosition.y - secondPosition.y;
	float z = firstPosition.z - secondPosition.z;

	//return (x * x + y * y + z * z) < distance * distance;
	return (x * x + z * z) < distance * distance;
}
