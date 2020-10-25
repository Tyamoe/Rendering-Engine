#pragma once

#include "Types.h"

#include "WindowUtils.h"

class Engine;
struct GLFWwindow;

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

		TYcstring GetName() { return name; }

		friend class TyRenderer;
		friend class Engine;

	private:
		TYcstring name;

		TYbool WindowInitialized = false;
		TYbool DirtyLayout = false;

		Settings settings;
		Layout layout;

		TYvoid cCreateWindow();

		static inline GLFWwindow* MainWindow = nullptr;
		static inline TYvector<Monitor> Monitors;
		static inline TYint PrimaryMonitor;

	protected:
		GLFWwindow* window = nullptr;

};

extern TYumap<GLFWwindow*, Window*> WindowManager;