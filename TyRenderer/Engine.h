#pragma once

#include "Types.h"

struct GLFWwindow;
class Window;
class Input;
class TyRenderer;
enum class RendererType;

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
		TyRenderer* renderer = nullptr;

		TYbool EngineInitialized = false;

		TYfloat dt = 0.0f;
		TYbool running = false;

		TYvoid Tick();
		TYvoid UpdateDT();

		TYbool WindowClosed();

	public:
		template<typename t>
		t GetWindow(t p = 0)
		{
			return t();
		}

		template<>
		GLFWwindow* GetWindow(GLFWwindow* p);

		template<>
		Window* GetWindow(Window* p);
};