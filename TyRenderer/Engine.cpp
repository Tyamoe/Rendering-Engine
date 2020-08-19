#include "stdafx.h"

#include <algorithm>

#include "Engine.h"
#include "Input.h"
#include "Renderer.h"
#include "RendererRayTraceCPU.h"
#include "RendererRayTrace.h"
#include "RendererEditor.h"
#include "RendererFlatColor.h"

void Engine::Init()
{
	for (TYsizet i = 0; i < renderers.size(); i++)
	{
		renderers[i]->Init();
	}
}

void Engine::Start()
{
	if (!running)
	{
		running = true;
		Tick();
	}
}

void Engine::Stop()
{
	running = false;
}

void Engine::Tick()
{
	if (!EngineInitialized) return;

	while (running && !glfwWindowShouldClose(window->window))
	{
		EarlyTick();

		for (TYsizet i = 0; i < renderers.size(); i++)
		{
			renderers[i]->Render(dt);
		}

		LateTick();
	}
}

void Engine::EarlyTick()
{
	if (!EngineInitialized) return;

	glfwPollEvents();

	UpdateDT();

	for (TYsizet i = 0; i < renderers.size(); i++)
	{
		renderers[i]->PreRender();
	}
}

void Engine::LateTick()
{
	if (!EngineInitialized) return;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glViewport(0, 0, GetWindow()->layout.width, GetWindow()->layout.height);
	glClearColor(0.3f, 0.69f, 0.35f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (TYsizet i = 0; i < renderers.size(); i++)
	{
		renderers[i]->PostRender();
	}

	window->input->Update(dt);

	glfwSwapBuffers(Gwindow);
}

void Engine::UpdateDT()
{
	dt = ImGui::GetIO().DeltaTime;
}

TYvoid Engine::AddRenderer(RendererPtr renderer)
{
	renderer->SetEngine(this);
	renderer->UpdatePriority(0);

	if (renderers.size() > 1)
	{
		RendererPtr r = renderers[0];
		renderers.erase(renderers.begin());
		delete r;
	}
	renderers.insert(renderers.begin(), renderer);

	renderer->Init();
}

RendererPtr Engine::GetRenderer(RendererType type)
{
	for (RendererPtr r : renderers)
	{
		if (r->type == type)
		{
			return r;
		}
	}
	return TYnull;
}

Engine::Engine(TYvector<RendererType> pRendererTypes)
{
	EngineInitialized = true;

	for (TYsizet i = 0; i < pRendererTypes.size(); i++)
	{
		RendererPtr renderer = nullptr;
		switch (pRendererTypes[i])
		{
		case RayTraceCPU:
			renderer = new RenderRayTraceCPU();
			renderer->SetEngine(this);
			renderer->UpdatePriority(0);
			break;
		case RayTrace:
			renderer = new RenderRayTrace();
			renderer->SetEngine(this);
			renderer->UpdatePriority(0);
			break;
		case Editor:
			renderer = new RenderEditor();
			renderer->SetEngine(this);
			renderer->UpdatePriority(2);
			break;
		case FlatColor:
			renderer = new RenderFlatColor();
			renderer->SetEngine(this);
			renderer->UpdatePriority(1);
			break;
		default:
			EngineInitialized = false;
			break;
		}

		renderers.push_back(renderer);
	}

	if (EngineInitialized)
	{
		std::sort(renderers.begin(), renderers.end(), RendererCMP());
	}
}

Engine::~Engine()
{
	if (EngineInitialized)
	{
		for (TYsizet i = 0; i < renderers.size(); i++)
		{
			delete renderers[i];
		}
	}
}