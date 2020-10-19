#include "Tyamoe3DHelper.h"

#ifndef TYAMOE3D

#include "stdafx.h"

#else

#include EngineInc(stdafx.h)

#endif // TYAMOE3D

#include "Camera.h"
#include "Globals.h"

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

	input = TY::in;

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
	TYfloat currSpeed = 50.0f * dt;
	input = TY::in;

	if (input->keyboard.keyCtrl)
	{
		currSpeed *= 0.2f;
	}
	if (input->keyboard.keyShift)
	{
		currSpeed *= 3.0f;
	}

	if (input->isKeyDown(GLFW_KEY_W))
	{
		position += currSpeed * front;

		iposition += currSpeed * front;

		dirty = true;
	}
	else if (input->isKeyDown(GLFW_KEY_S))
	{
		position -= currSpeed * front;

		iposition -= currSpeed * front;

		dirty = true;
	}
	if (input->isKeyDown(GLFW_KEY_A))
	{
		position -= right * currSpeed * multiplier;

		iposition -= right * currSpeed;

		dirty = true;
	}
	else if (input->isKeyDown(GLFW_KEY_D))
	{
		position += right * currSpeed * multiplier;

		iposition += right * currSpeed;

		dirty = true;
	}
	if (input->isKeyDown(GLFW_KEY_E))
	{
		position += currSpeed * up;

		iposition += currSpeed * up;

		dirty = true;
	}
	else if (input->isKeyDown(GLFW_KEY_Q))
	{
		position -= currSpeed * up;

		iposition -= currSpeed * up;

		dirty = true;
	}

	if (input->isKeyDown(GLFW_KEY_C))
	{
		yaw += input->mouse.screenOffset.x * sensitivity * multiplier;
		pitch += input->mouse.screenOffset.y * sensitivity;

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

	right = glm::normalize(glm::cross(front, up));

	view = glm::lookAt(position, position + front, up);
	
	iview = glm::lookAt(iposition, iposition + ifront, iup);

	dirty = false;

	return true;
}
