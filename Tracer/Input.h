#pragma once

#include "Types.h"

#include "InputUtils.h"

class Input
{
	public:
		Input(GLFWwindow* window);

		TYvoid Update(TYfloat dt);

		TYbool isKeyDown(TYint key);
		TYbool isKeyReleased(TYint key);
		TYbool isKeyPressed(TYint key);

		TYbool isMousePressed(MouseButton button);
		TYbool isMouseDown(MouseButton button);
		TYbool isMouseReleased(MouseButton button);

		TYvec2* getMouseWorldPosition();
		TYvoid UpdateMouseWorldPosition();

		static TYvoid KeyCB(GLFWwindow* window, TYint key, TYint scancode, TYint action, TYint mods);
		static TYvoid CursorCB(GLFWwindow* window, TYdouble xpos, TYdouble ypos);
		static TYvoid MouseCB(GLFWwindow* window, TYint button, TYint action, TYint mods);
		static TYvoid ScrollCB(GLFWwindow* window, TYdouble xoffset, TYdouble yoffset);

		Keyboard keyboard;
		Mouse mouse;
	private:
		GLFWwindow* window = nullptr;
};

typedef Input* InputPtr;
