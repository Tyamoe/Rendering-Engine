#include "stdafx.h"

#include "Camera.h"

static GLfloat mapToRange(GLfloat value, GLfloat ogRange1, GLfloat ogRange2, GLfloat newRange1, GLfloat newRange2)
{
	return (value - ogRange1) / (ogRange2 - ogRange1) * (newRange2 - newRange1) + newRange1;
}

Camera::~Camera()
{

}

Camera::Camera(InputPtr pInput, TYbool invert)
{
	input = pInput;
	inverted = invert;

	if (invert)
	{
		multiplier = -1.0f;
	}
}

Camera::Camera()
{

}

TYbool Camera::Update(TYfloat dt)
{
	float currSpeed = 4 * dt;

	if (input->isKeyDown(GLFW_KEY_W))
	{
		position += currSpeed * multiplier * front;

		iposition += currSpeed * front;

		dirty = true;
	}
	else if (input->isKeyDown(GLFW_KEY_S))
	{
		position -= currSpeed * multiplier * front;

		iposition -= currSpeed * front;

		dirty = true;
	}
	if (input->isKeyDown(GLFW_KEY_A))
	{
		position -= glm::normalize(glm::cross(front, up)) * currSpeed * multiplier;

		iposition -= glm::normalize(glm::cross(front, up)) * currSpeed;

		dirty = true;
	}
	else if (input->isKeyDown(GLFW_KEY_D))
	{
		position += glm::normalize(glm::cross(front, up)) * currSpeed * multiplier;

		iposition += glm::normalize(glm::cross(front, up)) * currSpeed;

		dirty = true;
	}
	if (input->isKeyDown(GLFW_KEY_E))
	{
		position += currSpeed * multiplier * up;

		iposition += currSpeed * up;

		dirty = true;
	}
	else if (input->isKeyDown(GLFW_KEY_Q))
	{
		position -= currSpeed * multiplier * up;

		iposition -= currSpeed * up;

		dirty = true;
	}

	if (input->isKeyDown(GLFW_KEY_C))
	{
		yaw += input->mouse.screenOffset.x * sensitivity * multiplier;
		pitch += input->mouse.screenOffset.y * sensitivity * multiplier;

		iyaw += input->mouse.screenOffset.x * sensitivity;
		ipitch += input->mouse.screenOffset.y * sensitivity;

		dirty = true;
	}

	if (!dirty)
	{
		return false;
	}

	if (pitch > 89.f) 
		pitch = 89.f;
	if (pitch < -89.f) 
		pitch = -89.f;

	if (ipitch > 89.f)
		ipitch = 89.f;
	if (ipitch < -89.f)
		ipitch = -89.f;

	TYvec temp;
	temp.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
	temp.y = sin(glm::radians(pitch));
	temp.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
	front = glm::normalize(temp);

	temp.x = cos(glm::radians(ipitch)) * cos(glm::radians(iyaw));
	temp.y = sin(glm::radians(ipitch));
	temp.z = cos(glm::radians(ipitch)) * sin(glm::radians(iyaw));
	ifront = glm::normalize(temp);

	view = glm::lookAt(position, position + front, up);

	iview = glm::lookAt(iposition, iposition + ifront, iup);

	dirty = false;

	return true;
}
