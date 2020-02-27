#pragma once

#include <random>

#include <cassert>
#include <condition_variable>
#include <mutex>

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

template<typename T>
T mix(T a, T b, T mix)
{
	return b * mix + a * (1 - mix);
}

class Barrier 
{
public:
	explicit Barrier(std::size_t iCount) :
		mThreshold(iCount),
		mCount(iCount),
		mGeneration(0) {
	}

	void Wait() 
	{
		std::unique_lock<std::mutex> lLock{ mMutex };
		auto lGen = mGeneration;
		if (!--mCount) 
		{
			mGeneration++;
			mCount = mThreshold;
			mCond.notify_all();
		}
		else 
		{
			mCond.wait(lLock, [this, lGen] { return lGen != mGeneration; });
		}
	}

private:
	std::mutex mMutex;
	std::condition_variable mCond;
	std::size_t mThreshold;
	std::size_t mCount;
	std::size_t mGeneration;
};

class Semaphore 
{
public:
	Semaphore(int count_ = 0)
		: count(count_) {}

	inline void notify()
	{
		std::unique_lock<std::mutex> lock(mtx);
		count++;
		cv.notify_one();
	}

	inline void wait()
	{
		std::unique_lock<std::mutex> lock(mtx);

		while (count == 0) {
			cv.wait(lock);
		}
		count--;
	}

private:
	std::mutex mtx;
	std::condition_variable cv;
	int count;
};