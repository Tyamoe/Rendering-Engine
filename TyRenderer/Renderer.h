#pragma once

#include "Types.h"

class Engine;
class Camera;
class Window;
class Shader;
class Scene;

enum class RendererType
{
	Base,
	RayTraceCPU,
	RayTraceVulkan,
	RayTrace,
	Editor,
	Deferred,
};

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

		Scene* scene;
		Camera* camera;

		friend struct RendererCMP;

	private:
		Shader* QuadShader = TYnull;

		TYuint RenderBuffer;
		TYuint RenderTexture;

		// Data
		//TYvector<EntityPtr> entities;

		// Debug
		//ProfilerPtr profiler;
		//DebuggerPtr debugger;

	protected:
		// (Meta) Data
		Window* window;

	public:
		TYvoid SetWindow(Window* pWindow) { window = pWindow; }
		//TYvoid AttachDebugger(DebuggerPtr pDebugger){ debugger = pDebugger;}
		//TYvoid AttachProfiler(ProfilerPtr pProfiler){ profiler = pProfiler;}
};