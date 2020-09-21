#pragma once

#include "Types.h"

#include "Window.h"
#include "Input.h"
#include "TyRenderer.h"

typedef class Window* WindowPtr;
typedef enum RendererType RendererType;

class Engine
{
	public:
		Engine();
		~Engine();

		void Start();
		void Break();

		void OnStart();

		void Update();
		void LateUpdate();

	private:
		TyRendererPtr renderer = nullptr;
		InputPtr input = nullptr;

		TYbool EngineInitialized = false;

		TYfloat dt = 0.0f;
		TYbool running = false;

		TYvoid Tick();
		TYvoid UpdateDT();

		TYbool WindowClosed();

	public:
		InputPtr GetInput()
		{
			return input;
		}

		template<typename t>
		t GetWindow(t p = 0)
		{
			return t();
		}

		template<>
		GLFWwindow* GetWindow(GLFWwindow* p)
		{
			return renderer->window->window;
		}

		template<>
		WindowPtr GetWindow(WindowPtr p)
		{
			return renderer->window;
		}
};

typedef Engine* EnginePtr;