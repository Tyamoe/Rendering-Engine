#pragma once

#include "Types.h"

class Input;

struct Keyboard
{
public:
	static inline TYbool keyCtrl;
	static inline TYbool keyShift;
	static inline TYbool keyAlt;

	friend class Input;

private:
	static inline TYbool keyDown[350];
	static inline TYbool keyReleased[350];
	static inline TYbool keyPressed[350];
};

struct Mouse
{
public:
	static inline TYvec2 screenPos;
	static inline TYvec2 screenOffset;

	friend class Input;

private:
	static inline TYbool mouseDown[5];
	static inline TYbool mouseReleased[5];
	static inline TYbool mousePressed[5];

	static inline TYfloat wheelHorizontal = 0.0f;
	static inline TYfloat wheelVertical = 0.0f;

	static inline TYfloat wheelVertDiff = 0.0f;

	static inline TYvec2 worldPos;
	static inline TYvec2 prevScreenPos;
};

typedef enum MouseButton
{
	MouseLeft,
	MouseRight,
	MouseMiddle,
}MouseButton;
