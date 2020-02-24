#pragma once

#include <random>

template<typename T>
T GetRand(T low, T high)
{
	if (low > high)
	{
		T temp = low;
		low = high;
		high = temp;
	}

	// fastest, but less accurate
	//return ((float(rand()) / float(RAND_MAX)) * (high - low)) + low;

	// super fast and super random
	static std::random_device rd;
	static std::mt19937 rng(rd());
	std::uniform_real_distribution<> dist(low, high);
	return (T)dist(rng);
}