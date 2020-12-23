#pragma once

#include "Types.h"

class Debugger
{
public:
	Debugger();
	~Debugger();

private:

};

typedef Debugger* DebuggerPtr;

namespace Debug
{
	inline TYbool DebugDraw;
	inline TYuint objCount;

	inline TYuint objDrawn;
	inline TYuint objSkipped;
	inline TYuint linesDrawn;
	inline TYuint linesSkipped;

	inline TYmap<TYstring, TYdouble> systemsPerfInfo;

	TYvoid Update(TYfloat dt);

	namespace Memory
	{
		inline TYint allocs;
		inline TYint deallocs;
		inline TYsizet bytesUsed;
		inline TYulong bytesInUse;
	}
}

#include <ctime>
#include <ratio>
#include <chrono>
using namespace std::chrono;

#ifdef _DEBUG
#define PERF_BENCHMARK_START  high_resolution_clock::time_point benchmark_clock_ = high_resolution_clock::now(); 
#define PERF_BENCHMARK_END \
	duration<double> benchmark_duration_ = duration_cast<duration<double>>(high_resolution_clock::now() - benchmark_clock_); \
	std::pair<std::string, double> benchmark_data_; \
	benchmark_data_.first = __FUNCTION__; \
	benchmark_data_.second = benchmark_duration_.count() * 1000; \
	Debug::systemsPerfInfo[benchmark_data_.first] += benchmark_data_.second; 
#else
//noops
#define PERF_BENCHMARK_START do { (TYvoid)sizeof(TYint); } while(0);
#define PERF_BENCHMARK_END do { (TYvoid)sizeof(TYint); } while(0);
#endif