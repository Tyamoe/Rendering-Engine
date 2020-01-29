#pragma once

#include "Types.h"

#include <ctime>
#include <ratio>
#include <chrono>

class Profiler
{
public:
	Profiler();
	~Profiler();

	void Start();
	void End();

private:
	TYfloat elapsed;

	TYbool stopped;
	TYbool paused;
};

typedef Profiler* ProfilerPtr;
