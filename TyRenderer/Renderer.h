#pragma once

#ifndef TYAMOE3D

#include "Types.h"
#include "Window.h"
#include "Scene.h"

#else

#include "Tyamoe3DHelper.h"
#include EngineInc(Window.h)
#include EngineInc(Types.h)
#include EngineInc(Scene.h)

#endif // TYAMOE3D

#include "GenericDraw.h"
#include "Camera.h"

typedef class Engine* EnginePtr;

typedef enum class RendererType
{
	Base,
	RayTraceCPU,
	RayTraceVulkan,
	RayTrace,
	Editor,
	Deferred,
} RendererType;

class Renderer
{
	public:
		Renderer();
		virtual ~Renderer();

		virtual TYvoid Init();

		virtual TYvoid PreRender();
		virtual TYvoid Render(TYfloat dt);
		virtual TYvoid PostRender();

		//TYvoid AddEntity(EntityPtr pEntity) { entities.push_back(pEntity); }
		//TYvoid AddEntity(TYvector<EntityPtr> pEntities) { entities.insert(entities.end(), pEntities.begin(), pEntities.end()); }

		TYvoid SetType(RendererType t) { type = t; }
		RendererType GetType() { return type; }
		RendererType type = RendererType::Base;

		ScenePtr scene;
		CameraPtr camera;

		friend struct RendererCMP;

	private:
		ShaderPtr QuadShader = TYnull;

		TYuint RenderBuffer;
		TYuint RenderTexture;

		// Data
		//TYvector<EntityPtr> entities;

		// Debug
		//ProfilerPtr profiler;
		//DebuggerPtr debugger;

	protected:
		// (Meta) Data
		WindowPtr window;

	public:
		TYvoid SetWindow(WindowPtr pWindow) { window = pWindow; }
		//TYvoid AttachDebugger(DebuggerPtr pDebugger){ debugger = pDebugger;}
		//TYvoid AttachProfiler(ProfilerPtr pProfiler){ profiler = pProfiler;}
};

typedef Renderer* RendererPtr;