#pragma once

struct Settings
{
	Settings(TYbool pVsync, TYint pMSAA) : vsync(pVsync), MSAA(pMSAA) {}

	TYbool vsync;
	TYint MSAA;
};
struct Layout
{
	Layout(TYint pW = 0, TYint pH = 0, TYint pL = 0, TYint pT = 0) : width(pW), height(pH), left(pL), top(pT) {}
	TYint width;
	TYint height;

	TYint left;
	TYint top;
};
