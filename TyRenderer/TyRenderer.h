#pragma once

#ifndef TYAMOE3D

#include "Types.h"

#else

#include "Tyamoe3DHelper.h"
#include EngineInc(Types.h)

#endif // TYAMOE3D

//#include "Window.h"
//#include "Engine.h"
#include "Renderer.h"

//typedef Renderer* RendererPtr;

class TyRenderer
{
public:
	TyRenderer();

	TYvoid Update(TYfloat dt);

	friend class Engine;

private:
	RendererPtr renderer = TYnull;
	WindowPtr window = TYnull;

	TYfloat currDT = 0.0f;

	TYvoid PreRender();
	TYvoid Render();
	TYvoid PostRender();
};

typedef TyRenderer* TyRendererPtr;