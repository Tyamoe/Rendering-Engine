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

		TYvoid AttachEngine(EnginePtr pEngine, bool pStart = true);
		TYvoid AttachInput(InputPtr pInput);

		TYbool Focus();

		GLFWwindow* GetGLFWWindow() { return window; }

		InputPtr GetInput() { return input; }

		Layout GetLayout() { return layout; }
		Layout& rLayout() { return layout; }

		TYbool GetDirtyLayout() { return DirtyLayout; }
		TYbool& rDirtyLayout() { return DirtyLayout; }

		friend class Engine;

	private:
		InputPtr input = nullptr;
		EnginePtr engine = nullptr;

		TYcstring name;

		TYbool WindowInitialized = false;
		TYbool DirtyLayout = false;

		TYbool fullscreen = false;
		TYbool maximize = false;
		TYbool vsync = false;
		TYint MSAA = 0;

		Layout layout;

		TYvoid cCreateWindow();

	protected:
		GLFWwindow* window = nullptr;

};

typedef Window* WindowPtr;

extern TYumap<GLFWwindow*, WindowPtr> WindowManager;