#pragma once

#include "stdafx.h"

#include "Types.h"
#include "Input.h"

typedef class Camera
{
public:
	Camera();
	Camera(InputPtr pInput, TYbool invert = false);
	~Camera();

	TYvoid Update(TYfloat dt);

	// Current View matrix.
	TYmat view = glm::mat4(1.0f);

	// Camera Transform -> used to make View matrix.
	TYvec position = TYvec(0.0f, 0.0f, -3.0f);
	TYvec front = TYvec(0.0f, 0.0f, -1.0f);
	TYvec up = TYvec(0.0f, 1.0f, 0.0f);

	TYfloat yaw = -91.69f, pitch = -17;
private:
	TYstring name;

	InputPtr input;

	TYbool inverted;

	TYfloat sensitivity = 3.7f;
	TYfloat multiplier = 1.0f;

}Camera, *CameraPtr;