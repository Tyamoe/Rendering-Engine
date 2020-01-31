#pragma once

#include "Types.h"

#include "Entity.h"
#include "Engine.h"
#include "Debugger.h"
#include "Profiler.h"

typedef class Engine* EnginePtr;

typedef struct RendererCMP RendererCMP;

enum RendererType
{
	RayTrace,
	Editor,
	FlatColor,
};

class Renderer
{
	public:
		//Renderer();
		virtual ~Renderer() {}

		virtual TYvoid Init() {}

		virtual TYvoid PreRender() {}
		virtual TYvoid Render(TYfloat dt) {}
		virtual TYvoid PostRender() {}

		TYvoid AddEntity(EntityPtr pEntity) { entities.push_back(pEntity); }
		TYvoid AddEntity(TYvector<EntityPtr> pEntities) { entities.insert(entities.end(), pEntities.begin(), pEntities.end()); }

		TYvoid UpdatePriority(TYint pPriority) { priority = pPriority; }

		friend struct RendererCMP;

	private:
		//  
		TYint priority = 0;

		// Data
		TYvector<EntityPtr> entities;

		// Debug
		ProfilerPtr profiler;
		DebuggerPtr debugger;

	protected:
		// (Meta) Data
		EnginePtr engine;

public:
	TYvoid SetEngine(EnginePtr pEngine) { engine = pEngine; }
	TYvoid AttachDebugger(DebuggerPtr pDebugger){ debugger = pDebugger;}
	TYvoid AttachProfiler(ProfilerPtr pProfiler){ profiler = pProfiler;}
};

typedef Renderer* RendererPtr;

struct RendererCMP
{
	TYbool operator()(RendererPtr a, RendererPtr b) const
	{
		return a->priority < b->priority;
	}
};