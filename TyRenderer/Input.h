#pragma once

#include "Types.h"

#include "InputUtils.h"

struct GLFWwindow;

class Input
{
	public:
		static TYvoid Update(TYfloat dt);

		static TYbool isKeyDown(TYint key);
		static TYbool isKeyReleased(TYint key);
		static TYbool isKeyPressed(TYint key);

		static TYbool isMousePressed(MouseButton button);
		static TYbool isMouseDown(MouseButton button);
		static TYbool isMouseReleased(MouseButton button);

		static TYvec2 GetMouseWorldPosition();

		static GLFWwindow* GetHighlightWindow();

		static TYvoid KeyCB(GLFWwindow* window, TYint key, TYint scancode, TYint action, TYint mods);
		static TYvoid CursorCB(GLFWwindow* window, TYdouble xpos, TYdouble ypos);
		static TYvoid MouseCB(GLFWwindow* window, TYint button, TYint action, TYint mods);
		static TYvoid ScrollCB(GLFWwindow* window, TYdouble xoffset, TYdouble yoffset);

		static inline Keyboard keyboard;
		static inline Mouse mouse;

	private:
		static inline GLFWwindow* highlightedWindow = nullptr;

		static TYvoid UpdateMouseWorldPosition();
};
