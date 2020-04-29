#pragma once

#include "Types.h"
#include "ThreadingUtils.h"

namespace Global
{
	inline TYbool GlewInit = false;
	inline TYbool GLFWInit = false;
	inline TYfloat FOV = 75.0f;

	inline TYint TriCount = 0;
	inline TYint CulledTries = 0;

	inline std::atomic<TYint> DevCounter = 0;
	inline TYbool DevBool = false;
	inline TYbool MultiThreadBool = true;

	inline TYint DevFrameCount = 0;

	inline TYint DevThreadCount = 100;
}