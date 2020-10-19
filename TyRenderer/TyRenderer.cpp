#include "stdafx.h"

#include "Window.h"
#include "GenericDraw.h"

#include "Debugger.h"

#include "TyRenderer.h"

#include "RendererRayTrace.h"
#include "RendererDeferred.h"
#include "RendererRayTraceCPU.h"

TyRenderer::TyRenderer()
{
	window = new Window("TyRenderer", Settings(), Layout(1000, 700, 200, 100));

	GenericDraw::Init();

	renderer = new RenderDeferred();
	renderer->SetWindow(window);
	renderer->Init();

	ImGuiContext* context = ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window->window, false);
	ImGui_ImplOpenGL3_Init();
	ImGui::StyleColorsDark();
}

TYvoid TyRenderer::Update(TYfloat dt)
{
	currDT = dt;

	PreRender();
	Render();
	PostRender();

	glfwSwapBuffers(window->window);
}

TYvoid TyRenderer::PreRender()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	renderer->PreRender();
}

TYvoid TyRenderer::Render()
{
	renderer->Render(currDT);
}

TYvoid TyRenderer::PostRender()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glViewport(0, 0, window->layout.width, window->layout.height);
	glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	renderer->PostRender();

	ImGui::Begin("Renderer");

	Debug::Update(currDT);

	if (ImGui::Button("Base"))
	{

	}
	else if (ImGui::Button("Deferred") && renderer->GetType() != RendererType::Deferred)
	{
		delete renderer;
		renderer = new RenderDeferred();
		renderer->SetWindow(window);
		renderer->Init();
	}
	else if (ImGui::Button("Raytrace") && renderer->GetType() != RendererType::RayTrace)
	{
		delete renderer;
		renderer = new RenderRayTrace();
		renderer->SetWindow(window);
		renderer->Init();
	}
	else if (ImGui::Button("RaytraceCPU") && renderer->GetType() != RendererType::RayTraceCPU)
	{
		delete renderer;
		renderer = new RenderRayTraceCPU();
		renderer->SetWindow(window);
		renderer->Init();
	}

	if (renderer->GetType() == RendererType::RayTrace && ImGui::Button("Recompile"))
	{
		((RenderRayTracePtr)renderer)->RecompileRaytracer();
	}

	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	ImGui::EndFrame();
}