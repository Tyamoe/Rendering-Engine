#pragma once

#ifndef TYAMOE3D

#include "Types.h"
#include "Scene.h"
#include "ThreadingUtils.h"

#else

#include "Tyamoe3DHelper.h"
#include EngineInc(Types.h)
#include EngineInc(Scene.h)
#include EngineInc(ThreadingUtils.h)

#endif // TYAMOE3D

#include "Renderer.h"
#include "Shader.h"
#include "Mesh.h"

#include "Camera.h"
#include "Geometry.h"
#include "RenderingUtils.h"

template <typename T>
struct atomwrapper
{
	std::atomic<T> _a;

	atomwrapper()
		:_a(0)
	{}

	atomwrapper(const std::atomic<T>& a)
		:_a(a.load())
	{}

	atomwrapper(const atomwrapper& other)
		:_a(other._a.load())
	{}

	atomwrapper& operator=(const atomwrapper& other)
	{
		_a.store(other._a.load());
		return *this;
	}
};

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

		TYuint AddMesh(Mesh& mesh);

		PixelColorF Trace(TYvec rayOrigin, TYvec rayDir, TYint rayDepth, TYint& rayCounter, TYint ignore);

		TYvoid ThreadHeight();
		TYvoid ThreadWidth();
		TYvoid ThreadGrid();
		TYvoid ThreadStrip();

		TraceData traceData;
		ScenePtr scene;

		CameraPtr camera;

		//
		RenderRayTraceCPU(const RenderRayTraceCPU& a) = delete;
		RenderRayTraceCPU& operator=(const RenderRayTraceCPU&) = delete;

		std::mutex lineLock;
		TYvector<TYvec> lines;

	private:
		ShaderPtr ColorShader = TYnull;
		ShaderPtr BloomShader = TYnull;
		ShaderPtr QuadShader = TYnull;

		TYuint RenderBuffer = 0;
		TYuint RenderTexture = 0;

		TYuint Frame = 0;

		TYuint PBO = 0;
		PixelColor* PixelBuffer = TYnull;

		TYint pixelCount = 0; 
		PixelColor clearColor = PixelColor(89, 155, 100, 255);

		TYvoid UpdateData();
		TYvoid TraceRays();
};

typedef RenderRayTraceCPU* RenderRayTraceCPUPtr;