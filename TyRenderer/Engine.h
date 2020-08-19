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
		TYvoid AddRenderer(RendererPtr renderer);

		RendererPtr GetRenderer(RendererType type);

		void Start();
		void Stop();

		void EarlyTick();
		void Tick();
		void LateTick();

	private:
		TYvector<RendererPtr> renderers;
		WindowPtr window = nullptr;
		GLFWwindow* Gwindow = nullptr;

		TYfloat dt = 0.0f;
		TYbool running = false;

		TYbool EngineInitialized = false;

	public:
		void SetWindow(WindowPtr pWindow, GLFWwindow* Gwindow_) 
		{ 
			window = pWindow; 
			Gwindow = Gwindow_;
		}
		GLFWwindow* GetGLFWWindow() { return Gwindow; }
		WindowPtr GetWindow() { return window; }

	private:
		void UpdateDT();
};

typedef Engine* EnginePtr;