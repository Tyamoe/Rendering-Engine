#pragma once

#include "Types.h"

class Camera
{
public:
	Camera();
	Camera(TYbool invert = false);
	~Camera();
	TYbool Update(TYfloat dt);

	TYbool dirty = true;

	// Current View matrix.
	TYmat view = TYmat(1.0f);
	TYmat proj = TYmat(1.0f);

	// Camera Transform -> used to make View matrix.
	TYvec position = TYvec(-0.45f, 0.5f, 20.0f);
	TYvec front = TYvec(0.0f, 0.0f, -1.0f);
	TYvec up = TYvec(0.0f, 1.0f, 0.0f);

	TYvec right = TYvec(1.0f, 0.0f, 0.0f);
	TYvec2 dim = TYvec2(0.0f, 0.0f);

	TYfloat near = 0.1f;
	TYfloat far = 2000.0f;

	friend class GenericDraw;

private:
	TYstring name;

	TYbool inverted;

	TYfloat sensitivity = 4.5f;
	TYfloat multiplier = 1.0f;

	TYfloat yaw = -90.0f, pitch = 0.0f;
};