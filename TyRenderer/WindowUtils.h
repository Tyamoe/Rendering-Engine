#pragma once

#include "Types.h"

struct Settings
{
	Settings(TYbool pMaximize = false, TYbool pFullscreen = false, TYbool pVsync = false, TYint pMSAA = 0) :
		maximize(pMaximize), fullscreen(pFullscreen), vsync(pVsync), MSAA(pMSAA) {}

	TYbool vsync;
	TYint MSAA;
	TYbool maximize;
	TYbool fullscreen;
};

struct Layout
{
	Layout(TYint pW = 800, TYint pH = 600, TYint pL = 0, TYint pT = 0) : width(pW), height(pH), left(pL), top(pT) {}
	TYint width;
	TYint height;

	TYint left;
	TYint top;
};

struct Monitor
{
	TYint width;
	TYint height;

	TYint refreshRate;
};