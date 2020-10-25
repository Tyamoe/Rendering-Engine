#include "Engine.h"

#include <algorithm>

#include "Input.h"
#include "Renderer.h"
#include "Window.h"
#include "TyRenderer.h"

#include "GLUtils.h"
#include "ImGuiUtils.h"

Engine::Engine()
{
	renderer = new TyRenderer();

	GLFWwindow* win = GetWindow<GLFWwindow*>();

	EngineInitialized = true;
}

TYvoid Engine::Start()
{
	if (!running)
	{
		running = true;

		OnStart();
		Tick();
	}
}

TYvoid Engine::Break()
{
	running = false;
}

TYvoid Engine::OnStart()
{

}

TYvoid Engine::Tick()
{
	if (!EngineInitialized) return;

	ImGui::LoadIniSettingsFromDisk("imgui.ini");

	while (running && !WindowClosed())
	{
		Update();
		LateUpdate();
	}

	ImGui::SaveIniSettingsToDisk("imgui.ini");
}

TYvoid Engine::Update()
{
	if (!EngineInitialized) return;

	glfwPollEvents();
	UpdateDT();
}

TYvoid Engine::LateUpdate()
{
	if (!EngineInitialized) return;

	renderer->Update(dt);

	Input::Update(dt);
}

TYvoid Engine::UpdateDT()
{
	dt = ImGui::GetIO().DeltaTime;
}

TYbool Engine::WindowClosed()
{
	GLFWwindow* w = GetWindow<GLFWwindow*>();
	return glfwWindowShouldClose(w);
}

template<>
GLFWwindow* Engine::GetWindow(GLFWwindow* p)
{
	return renderer->window->window;
}

template<>
Window* Engine::GetWindow(Window* p)
{
	return renderer->window;
}

Engine::~Engine()
{
	if (EngineInitialized)
	{

	}
}