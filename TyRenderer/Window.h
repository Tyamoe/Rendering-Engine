#pragma once

#include "stdafx.h"

#include "Types.h"

#include "WindowUtils.h"
#include "Input.h"

typedef class Engine* EnginePtr;

class Window
{
	public:
		Window(TYcstring pName, Settings pSettings, Layout pLayout);
		~Window();

		TYbool Focus();

		GLFWwindow* GetGLFWWindow() { return window; }

		Layout GetLayout() { return layout; }
		Layout& rLayout() { return layout; }

		TYbool GetDirtyLayout() { return DirtyLayout; }
		TYbool& rDirtyLayout() { return DirtyLayout; }

		friend class TyRenderer;
		friend class Engine;

	private:
		TYcstring name;

		TYbool WindowInitialized = false;
		TYbool DirtyLayout = false;

		TYbool fullscreen = false;
		TYbool maximize = false;
		TYbool vsync = false;
		TYint MSAA = 0;

		Settings settings;
		Layout layout;

		TYvoid cCreateWindow();

	protected:
		GLFWwindow* window = nullptr;

};

typedef Window* WindowPtr;

#ifdef HEADLESS

extern TYumap<GLFWwindow*, WindowPtr> WindowManager;

#else

#endif // HEADLESS