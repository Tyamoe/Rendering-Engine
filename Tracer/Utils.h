#pragma once

#include <random>

#include <cassert>
#include <condition_variable>
#include <mutex>

#include "Types.h"

template<typename T>
T GetRand(T low, T high, TYbool fast = false)
{
	if (fast)
	{
		return ((TYfloat(rand()) / TYfloat(RAND_MAX)) * (high - low)) + low;
	}

	static std::random_device rd;
	static std::mt19937 rng(rd());
	std::uniform_real_distribution<> dist(low, high);

	return (T)dist(rng);
}

template<typename T>
T Mix(T a, T b, T mix)
{
	return b * mix + a * (1 - mix);
}
