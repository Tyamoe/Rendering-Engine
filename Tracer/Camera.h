#pragma once

#include "stdafx.h"

#include "Input.h"

typedef class Camera
{
public:
	Camera();
	Camera(InputPtr pInput, TYbool invert = false);
	~Camera();

	void Update(float dt);

	glm::vec2 TargetWorldPosition;

	// Current View matrix.
	glm::mat4 view = glm::mat4(1.0f);

	// Camera Transform -> used to make View matrix.
	glm::vec3 Position = glm::vec3(-4.30f, 5.5f, 6.4f);
	glm::vec3 Front = glm::vec3(0.5f, -0.5f, -0.6f);
	glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);

	float yaw_ = -83.4f, pitch_ = -35;
private:
	std::string name;

	TYbool inverted;
	TYfloat multiplier = 1.0f;

	InputPtr input;

	glm::vec2 Target;

	// Camera zoom.
	float zoom = 25.f;

	float speed = 80;
	//the number is multiplied by 200 to give more granularity to the user
	float smoothness = 100 * .05f;
	float smoothDiv = 200;

	// Current Projection matrix.
	glm::mat4 proj;
	// Projection/View matrix concat
	glm::mat4 PV;

}Camera, *CameraPtr;