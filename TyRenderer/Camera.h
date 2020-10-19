#pragma once

#ifndef TYAMOE3D

#include "stdafx.h"
#include "Types.h"
#include "Input.h"

#else

#include "Tyamoe3DHelper.h"
#include EngineInc(Types.h)
#include EngineInc(Input.h)

#endif // TYAMOE3D

class Camera
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
	TYvec position = TYvec(-0.45f, 0.5f, 20.0f);
	TYvec front = TYvec(0.0f, 0.0f, -1.0f);
	TYvec up = TYvec(0.0f, 1.0f, 0.0f);

	TYvec right = TYvec(1.0f, 0.0f, 0.0f);
	TYvec2 dim = TYvec2(0.0f, 0.0f);

	TYfloat yaw = -90.0f, pitch = 0.0f;

	TYvec iposition = TYvec(0.0f, 0.0f, 20.0f);
	TYvec ifront = TYvec(0.0f, 0.0f, -1.0f);
	TYvec iright = TYvec(1.0f, 0.0f, 0.0f);
	TYvec iup = TYvec(0.0f, 1.0f, 0.0f);

	TYmat iview = TYmat(1.0f);

	TYfloat iyaw = -90.0f;
	TYfloat ipitch = 0.0f;
private:
	TYstring name;

	InputPtr input;

	TYbool inverted;

	TYfloat sensitivity = 4.5f;
	TYfloat multiplier = 1.0f;
};

typedef class Camera* CameraPtr;