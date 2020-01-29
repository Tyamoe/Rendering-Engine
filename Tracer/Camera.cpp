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

Camera::Camera()
{
	Target = Position;
}

extern bool ass;
void Camera::Update(float dt = 0.0167f)
{
	float currSpeed = 2 * dt;
	if (Input::isKeyDown(GLFW_KEY_W))
		Position += currSpeed * Front;
	if (Input::isKeyDown(GLFW_KEY_S))
		Position -= currSpeed * Front;
	if (Input::isKeyDown(GLFW_KEY_A))
		Position -= glm::normalize(glm::cross(Front, Up)) * currSpeed;
	if (Input::isKeyDown(GLFW_KEY_D))
		Position += glm::normalize(glm::cross(Front, Up)) * currSpeed;

	if (Input::isKeyDown(GLFW_KEY_C))
	{
		yaw_ += Input::mouse.screenOffset.x;
		pitch_ += Input::mouse.screenOffset.y;
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

void Camera::getWorldPosition(float* x, float* y)
{
	float h = 1.0f / zoom;
	h = Position.x * h;
	float q = 1.0f / zoom;
	q = Position.y * q;

	*x = h;
	*y = q;

	*x = Position.x;
	*y = Position.y;
}

void Camera::ChangeCameraZoom(float scale)
{
	glm::vec3 currPos = Position;

	Position = glm::vec3(0);

	zoom = scale;

	Position = currPos;
}
