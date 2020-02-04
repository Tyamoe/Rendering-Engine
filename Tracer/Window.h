#pragma once

#include "stdafx.h"
#include "Types.h"
#include "Structs.h"

//#include "Engine.h"
#include "Input.h"

typedef class Engine* EnginePtr;

class Window
{
	public:
		Window(Settings pSettings, Layout pLayout);
		~Window();

		void AttachEngine(EnginePtr pEngine);
		void AttachInput(InputPtr pInput);

		GLFWwindow* GetGLFWWindow() { return window; }
		Layout GetLayout() { return layout; }

		friend class Engine;

	private:
		InputPtr input = nullptr;
		EnginePtr engine = nullptr;

		TYbool WindowInitialized = false;

		TYbool vsync = false;
		TYint MSAA = 0;

		Layout layout;

		void CreateWindow(Layout layout);

	protected:
		GLFWwindow* window = nullptr;

};

typedef Window* WindowPtr;