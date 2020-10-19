#include "stdafx.h"

#include <algorithm>

#include "Engine.h"
#include "Input.h"
#include "Renderer.h"

Engine::Engine()
{
	renderer = new TyRenderer();
	GLFWwindow* w = GetWindow<GLFWwindow*>();
	input = new Input(w);

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
	input->Update(dt);
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

Engine::~Engine()
{
	if (EngineInitialized)
	{

	}
}