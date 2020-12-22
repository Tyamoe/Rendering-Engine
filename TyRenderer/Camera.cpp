#include "Camera.h"

#include "Input.h"
#include "GenericDraw.h"
#include "TyRenderer.h"

#include "GLUtils.h"
#include "Globals.h"

static TYfloat mapToRange(TYfloat value, TYfloat ogRange1, TYfloat ogRange2, TYfloat newRange1, TYfloat newRange2)
{
	return (value - ogRange1) / (ogRange2 - ogRange1) * (newRange2 - newRange1) + newRange1;
}

Camera::~Camera()
{

}

Camera::Camera(TYbool invert)
{
	inverted = invert;

	if (invert)
	{
		multiplier = -1.0f;
	}
	GenericDraw::MainCamera = this;
}

Camera::Camera()
{
	GenericDraw::MainCamera = this;
}

TYbool Camera::Update(TYfloat dt)
{
	TYfloat currSpeed = 50.0f * dt;

	if (Input::keyboard.keyCtrl && Input::keyboard.keyShift)
	{
		currSpeed *= 10.0f;
	}
	else
	{
		if (Input::keyboard.keyCtrl)
		{
			currSpeed *= 0.1f;
		}
		if (Input::keyboard.keyShift)
		{
			currSpeed *= 3.0f;
		}
	}


	if (Input::isKeyDown(GLFW_KEY_W))
	{
		position += currSpeed * front;
		dirty = true;
	}
	else if (Input::isKeyDown(GLFW_KEY_S))
	{
		position -= currSpeed * front;
		dirty = true;
	}
	if (Input::isKeyDown(GLFW_KEY_A))
	{
		position -= right * currSpeed * multiplier;
		dirty = true;
	}
	else if (Input::isKeyDown(GLFW_KEY_D))
	{
		position += right * currSpeed * multiplier;
		dirty = true;
	}
	if (Input::isKeyDown(GLFW_KEY_E))
	{
		position += currSpeed * up;
		dirty = true;
	}
	else if (Input::isKeyDown(GLFW_KEY_Q))
	{
		position -= currSpeed * up;
		dirty = true;
	}

	if (Input::isKeyDown(GLFW_KEY_C))
	{
		yaw += Input::mouse.screenOffset.x * sensitivity * multiplier;
		pitch += Input::mouse.screenOffset.y * sensitivity;

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

	TYvec temp;
	temp.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
	temp.y = sin(glm::radians(pitch));
	temp.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
	front = glm::normalize(temp);

	right = glm::normalize(glm::cross(front, up));

	view = glm::lookAt(position, position + front, up);
	proj = glm::perspective(glm::radians(Global::FOV), TYfloat(TyRenderer::width) / TYfloat(TyRenderer::height), near, far);

	dirty = false;

	return true;
}
