#pragma once

#include "Types.h"

#include "Renderer.h"
#include "Window.h"

typedef class Renderer* RendererPtr;
typedef class Window* WindowPtr;

class Engine
{
public:
	Engine();
	~Engine();

private:
	RendererPtr renderer;
	WindowPtr window;
};

typedef Engine* EnginePtr;