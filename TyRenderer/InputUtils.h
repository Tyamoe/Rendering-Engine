#pragma once

#include "Types.h"

struct Keyboard
{
	TYbool keyDown[350];
	TYbool keyReleased[350];
	TYbool keyPressed[350];
	TYbool keyCtrl;
	TYbool keyShift;
	TYbool keyAlt;
};

struct Mouse
{
	TYbool mouseDown[5];
	TYbool mouseReleased[5];
	TYbool mousePressed[5];

	TYfloat wheelHorizontal = 0.0f;
	TYfloat wheelVertical = 0.0f;

	TYfloat wheelVertDiff = 0.0f;

	TYvec2 screenPos;
	TYvec2 worldPos;
	TYvec2 prevScreenPos;
	TYvec2 screenOffset;
};

typedef enum MouseButton
{
	MouseLeft,
	MouseRight,
	MouseMiddle,
}MouseButton;
