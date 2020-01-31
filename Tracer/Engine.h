#pragma once

#include "Types.h"

#include "Renderer.h"
#include "Window.h"

typedef class Renderer* RendererPtr;
typedef class Window* WindowPtr;
typedef enum RendererType RendererType;

class Engine
{
	public:
		Engine(TYvector<RendererType> pRendererTypes);
		~Engine();

		TYvoid Init();

		void Start();
		void Stop();

		void EarlyTick();
		void Tick();
		void LateTick();

	private:
		TYvector<RendererPtr> renderers;
		WindowPtr window = nullptr;

		TYfloat dt = 0.0f;
		TYbool running = false;

		TYbool EngineInitialized = false;

	public:
		void SetWindow(WindowPtr pWindow) { window = pWindow; }
		GLFWwindow* GetWindow() { return window->GetGLFWWindow(); }

	private:
		void UpdateDT();
};

typedef Engine* EnginePtr;