#include "stdafx.h"

#include "Camera.h"
#include "Render.h"
#include "Input.h"

static GLfloat mapToRange(GLfloat value, GLfloat ogRange1, GLfloat ogRange2, GLfloat newRange1, GLfloat newRange2)
{
	return (value - ogRange1) / (ogRange2 - ogRange1) * (newRange2 - newRange1) + newRange1;
}

Camera::~Camera()
{

}

Camera::Camera(InputPtr pInput, TYbool invert)
{
	Target = Position;

	input = pInput;
	inverted = invert;

	if (invert)
	{
		multiplier = -1.0f;
	}
}

Camera::Camera()
{
	Target = Position;
}

extern bool ass;
void Camera::Update(float dt = 0.0167f)
{
	float currSpeed = 2 * dt;
	if (input->isKeyDown(GLFW_KEY_W))
		Position += currSpeed * multiplier * Front;
	if (input->isKeyDown(GLFW_KEY_S))
		Position -= currSpeed * multiplier * Front;
	if (input->isKeyDown(GLFW_KEY_A))
		Position -= glm::normalize(glm::cross(Front, Up)) * currSpeed * multiplier;
	if (input->isKeyDown(GLFW_KEY_D))
		Position += glm::normalize(glm::cross(Front, Up)) * currSpeed * multiplier;
	if (input->isKeyDown(GLFW_KEY_E))
		Position += currSpeed * multiplier * Up;
	if (input->isKeyDown(GLFW_KEY_Q))
		Position -= currSpeed * multiplier * Up;

	if (input->isKeyDown(GLFW_KEY_C))
	{
		yaw_ += input->mouse.screenOffset.x * multiplier;
		pitch_ += input->mouse.screenOffset.y * multiplier;
	}

	if (pitch_ > 89.f) 
		pitch_ = 89.f;
	if (pitch_ < -89.f) 
		pitch_ = -89.f;

	glm::vec3 temp;
	temp.x = cos(glm::radians(pitch_)) * cos(glm::radians(yaw_));
	temp.y = sin(glm::radians(pitch_));
	temp.z = cos(glm::radians(pitch_)) * sin(glm::radians(yaw_));
	Front = glm::normalize(temp);

	view = glm::lookAt(Position, Position + Front, Up);
}
