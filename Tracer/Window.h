#pragma once

#include "Types.h"

#include "Engine.h"

typedef class Engine* EnginePtr;

class Window
{
	EnginePtr engine;
};

typedef Window* WindowPtr;