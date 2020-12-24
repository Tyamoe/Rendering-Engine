#pragma once

#include "Types.h"
#include "ThreadingUtils.h"

namespace Global
{
	inline TYbool GlewInit = false;
	inline TYbool GLFWInit = false;
	inline TYfloat FOV = 75.0f;

	inline std::atomic<TYint> TriCount = 0;
	inline std::atomic<TYint> CulledTries = 0;

	inline std::atomic<TYint> DevCounter = 0;
	inline TYbool DevBool = false;
	inline TYbool DevBool2 = false;
	inline TYbool DevSphereAABB = false;
	inline TYbool MultiThreadBool = true;

	inline TYint DevFrameCount = 0;

	inline TYint DevThreadCount = 100;

	inline TYint DevComputeShaderI = 0;
	inline TYbool DevComputeShaderB = false;
	inline TYfloat DevComputeShaderF = 1.0f;
	inline TYvec DevComputeShaderV = TYvec();

	inline TYbool OctreeIntersect = true;
	inline TYbool seeIntersect = false;
}