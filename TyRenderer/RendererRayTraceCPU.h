#pragma once

#include "Types.h"

#include "Renderer.h"

#include "RenderingUtils.h"
#include "ThreadingUtils.h"

class Shader;

class RenderRayTraceCPU : public Renderer
{
	struct TraceData
	{
		TYbool tracing = true;

		TYfloat width = 0;
		TYfloat height = 0;

		TYint ThreadCount = 0;

		TYvec Origin = TYvec(0.0f);
		TYvector3 Direction;

		TYfloat aspect;
		TYfloat angle;

		TYfloat invWidth;
		TYfloat invHeight;

		TYbool dirty = true;
	};

	public:
		struct // Threading
		{
			Semaphore* block = TYnull;
			Semaphore* traceWait = TYnull;

			Barrier* traceBarrier = TYnull;

			std::mutex countLock;

			std::atomic<TYint> count = 0;

			std::thread* tracingThreads = TYnull;
			TYint threadCount = 0;
	};

	public:
		RenderRayTraceCPU();
		~RenderRayTraceCPU();

		TYvoid Init();

		TYvoid PreRender();
		TYvoid Render(TYfloat dt);
		TYvoid PostRender();

		PixelColorF Trace(TYvec rayOrigin, TYvec rayDir, TYint rayDepth, TYint& rayCounter, TYint ignore);

		TYvoid ThreadHeight();
		TYvoid ThreadWidth();
		TYvoid ThreadGrid();
		TYvoid ThreadStrip();

		TraceData traceData;

		//
		RenderRayTraceCPU(const RenderRayTraceCPU& a) = delete;
		RenderRayTraceCPU& operator=(const RenderRayTraceCPU&) = delete;

		std::mutex lineLock;
		TYvector<TYvec> lines;

	private:
		Shader* ColorShader = TYnull;
		Shader* BloomShader = TYnull;
		Shader* QuadShader = TYnull;

		TYuint RenderBuffer;
		TYuint RenderTexture;

		TYuint Frame = 0;

		TYuint PBO = 0;
		PixelColor* PixelBuffer = TYnull;

		TYint pixelCount = 0; 
		PixelColor clearColor = PixelColor(89, 155, 100, 255);

		TYvoid UpdateData();
		TYvoid TraceRays();
};