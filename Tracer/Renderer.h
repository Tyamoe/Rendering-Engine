#pragma once

#include "Types.h"

#include "Entity.h"
#include "Engine.h"
#include "Debugger.h"
#include "Profiler.h"

typedef class Engine* EnginePtr;

class Renderer
{
	public:
		//Renderer();
		virtual ~Renderer() {}

		virtual void PreRender() {}
		virtual void Render() {}
		virtual void PostRender() {}

	private:
		//  
		TYint priority;

		// Data
		TYvector<EntityPtr> entities;

		// Debug
		ProfilerPtr profiler;
		DebuggerPtr debugger;

		// (Meta) Data
		EnginePtr engine;
};

typedef Renderer* RendererPtr;