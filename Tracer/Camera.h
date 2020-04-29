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

	TYbool Update(TYfloat dt);

	TYbool dirty = true;

	// Current View matrix.
	TYmat view = TYmat(1.0f);

	// Camera Transform -> used to make View matrix.
	TYvec position = TYvec(0.0f, 0.0f, -3.0f);
	TYvec front = TYvec(0.0f, 0.0f, -1.0f);
	TYvec up = TYvec(0.0f, 1.0f, 0.0f);

	TYfloat yaw = -91.69f, pitch = -17;

	TYvec iposition = TYvec(0.0f, 0.0f, -3.0f);
	TYvec ifront = TYvec(0.0f, 0.0f, -1.0f);
	TYvec iup = TYvec(0.0f, 1.0f, 0.0f);
	TYfloat iyaw = -91.69f;
	TYfloat ipitch = -17;
	TYmat iview = TYmat(1.0f);
private:
	TYstring name;

	InputPtr input;

	TYbool inverted;

	TYfloat sensitivity = 3.7f;
	TYfloat multiplier = 1.0f;

}Camera, *CameraPtr;