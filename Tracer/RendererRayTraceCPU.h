#pragma once

#include "Types.h"

#include "Renderer.h"
#include "Shader.h"
#include "Mesh.h"

#include "Camera.h"
#include "Geometry.h"
#include "RenderingUtils.h"
#include "ThreadingUtils.h"

class RenderRayTraceCPU : public Renderer
{
	struct TraceData
	{
		TYbool tracing = true;

		TYint width = 0;
		TYint height = 0;

		TYvec Origin = TYvec(0.0f);
		TYvector3 Direction;

	};

	public:
		struct // Threading
		{
			Semaphore* block = TYnull;
			Semaphore* traceWait = TYnull;

			Barrier* traceBarrier = TYnull;

			std::mutex countLock;

			std::atomic<TYint> count = 0;
	};

	public:
		RenderRayTraceCPU();
		~RenderRayTraceCPU();

		TYvoid Init();

		TYvoid PreRender();
		TYvoid Render(TYfloat dt);
		TYvoid PostRender();

		TYuint AddMesh(Mesh& mesh);

		PixelColorF Trace(TYvec rayOrigin, TYvec rayDir, TYint rayDepth, TYint modifier = 0);

		TraceData traceData;
		TYvector<Geometry*> Scene;

	private:
		CameraPtr camera;

		ShaderPtr BloomShader = TYnull;
		ShaderPtr QuadShader = TYnull;

		TYuint RenderBuffer = 0;
		TYuint RenderTexture = 0;

		TYuint Frame = 0;

		TYuint PBO = 0;
		PixelColor* PixelBuffer = TYnull;

		TYint pixelCount = 0; 
		PixelColor clearColor = PixelColor(89, 155, 100, 255);

		std::thread* tracingThreads = TYnull;
		TYint threadCount = 0;

		TYvoid UpdateData();
		TYvoid TraceRays();
};

typedef RenderRayTraceCPU* RenderRayTraceCPUPtr;