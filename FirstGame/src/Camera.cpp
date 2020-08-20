#include "Camera.h"
#include <iostream>

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
{
	speedUp = 5.0f;
	//Position = position; 
	Position = glm::vec3(0.0f, 0.0f, 0.0f); 
	//finalCameraPoint = Position.y;
	finalCameraPoint = 300.0f;
	WorldUp = up;
	Yaw = yaw;
	Pitch = pitch;
	updateCameraVectors();
}

Camera::~Camera()
{
}

glm::mat4 Camera::GetViewMatrix() const
{
	return glm::lookAt(Position, Position + Front, Up);
}


void Camera::updateCameraVectors()
{
	glm::vec3 front;
	front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	front.y = sin(glm::radians(Pitch));
	front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	Front = glm::normalize(front);

	Right = glm::normalize(glm::cross(Front, WorldUp));
	Up = glm::normalize(glm::cross(Right, Front));
}

void Camera::menuMove(const float& deltaTime)
{
	static float counter = 0.0f;

	counter += deltaTime * 5.0f;

	if (counter > 360.0f) counter = 0.0f;

	float x = sin(glm::radians(counter)) * 700.0f;
	float z = cos(glm::radians(counter)) * 700.0f;
	
	Position.x = x;
	Position.z = z;
}