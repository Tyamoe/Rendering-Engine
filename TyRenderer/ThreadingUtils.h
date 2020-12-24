#pragma once

#include "Types.h"

#include <thread>
#include <mutex>
#include <atomic>

class Barrier
{
public:
	explicit Barrier(TYsizet pCount) : bThreshold(pCount), bCount(pCount), bGeneration(0) {}

	TYvoid Wait()
	{
		std::unique_lock<std::mutex> lock{ bMutex };
		TYsizet gen = bGeneration;
		if (!--bCount)
		{
			bGeneration++;
			bCount = bThreshold;
			bCond.notify_all();
		}
		else
		{
			bCond.wait(lock, [this, gen] { return gen != bGeneration; });
		}
	}

private:
	TYsizet bCount;
	TYsizet bThreshold;
	TYsizet bGeneration;

	std::mutex bMutex;
	std::condition_variable bCond;
};

class Semaphore
{
public:
	Semaphore(TYsizet pCount = 0) : sCount(pCount) {}

	TYvoid Wait()
	{
		std::unique_lock<std::mutex> lock(sMutex);

		while (sCount == 0)
		{
			sCond.wait(lock);
		}
		sCount--;
	}

	TYvoid Notify()
	{
		std::unique_lock<std::mutex> lock(sMutex);
		sCount++;
		sCond.notify_one();
	}

	TYvoid NotifyAll(TYsizet pCount)
	{
		std::unique_lock<std::mutex> lock(sMutex);
		sCount = pCount;
		sCond.notify_all();
	}

private:
	TYsizet sCount;

	std::mutex sMutex;
	std::condition_variable sCond;
};

template <typename T>
struct TyAtomic
{
	std::atomic<T> value;

	TyAtomic() : value(0) {}
	TyAtomic(const std::atomic<T>& v) : value(v.load()) {}
	TyAtomic(const TyAtomic& v) : value(v.value.load()) {}

	TyAtomic& operator=(const TyAtomic& v)
	{
		value.store(v.value.load());
		return *this;
	}
};
