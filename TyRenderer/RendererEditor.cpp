#include "Tyamoe3DHelper.h"

#ifndef TYAMOE3D

#include "stdafx.h"
#include "Globals.h"

#else

#include EngineInc(stdafx.h)
#include EngineInc(Globals.h)

#endif // TYAMOE3D

#include <filesystem>

#include "RendererRayTrace.h"
#include "RendererRayTraceCPU.h"
#include "RendererEditor.h"

using namespace std::filesystem;

TYvoid RenderEditor::PreRender()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

TYvoid RenderEditor::Render(TYfloat dt)
{
	ImGui::Begin("Settings");
	TYfloat fps = ImGui::GetIO().Framerate;
	fps = 1.0f / dt;

	ImGui::InputFloat("FPS", &fps, -180.0f, 1080.0f);

	ImGui::Text("Tri %d", Global::TriCount.load());
	ImGui::Text("Culled %d", Global::CulledTries.load());
	ImGui::Text("Rays %d", Global::DevCounter.operator int());

	ImGui::InputInt("Thread Count", &Global::DevThreadCount);

	ImGui::SliderFloat("FOV", &Global::FOV, -180.0f, 180.0f);
	ImGui::Checkbox("Dev Bool", &Global::DevBool);
	ImGui::Checkbox("Dev Bool2", &Global::DevBool2);
	ImGui::Checkbox("Sphere Bound", &Global::DevSphereAABB);

	ImGui::Text("Compute Shader");
	ImGui::Checkbox("Bool", &Global::DevComputeShaderB);
	ImGui::SliderInt("Int", &Global::DevComputeShaderI, 0, 10);
	ImGui::SliderFloat("Float", &Global::DevComputeShaderF, -50.0f, 50.0f);
	ImGui::SliderFloat3("Vec3", &Global::DevComputeShaderV[0], 0.0f, 1.0f);

	if (ImGui::Button("Recompile Tracer"))
	{
		RenderRayTracePtr a = static_cast<RenderRayTracePtr>(engine->GetRenderer(RayTrace));
		if(a)
			a->RecompileRaytracer();
	}

	//ImGui::Checkbox("Use MultiThreading", &Global::MultiThreadBool);
	/*if (ImGui::Button("Height"))
	{
		RenderRayTraceCPUPtr r = new RenderRayTraceCPU();
		engine->AddRenderer(r);
		r->ThreadHeight();
	}
	else if (ImGui::Button("Width"))
	{
		RenderRayTraceCPUPtr r = new RenderRayTraceCPU();
		engine->AddRenderer(r);
		r->ThreadWidth();
	}
	else if (ImGui::Button("Grid"))
	{
		RenderRayTraceCPUPtr r = new RenderRayTraceCPU();
		engine->AddRenderer(r);
		r->ThreadGrid();
	}
	else if (ImGui::Button("Strip"))
	{
		RenderRayTraceCPUPtr r = new RenderRayTraceCPU();
		engine->AddRenderer(r);
		r->ThreadStrip();
	}*/

	ImGui::End();
}

TYvoid RenderEditor::PostRender()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	ImGui::EndFrame();
}

TYvoid RenderEditor::Init()
{
	context = ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(engine->GetGLFWWindow(), false);
	ImGui_ImplOpenGL3_Init();
	ImGui::StyleColorsDark();
}

RenderEditor::RenderEditor(TYbool pReadonly, Windows pActiveWindows) : Renderer()
{
	SetType(Editor);

	readonly = pReadonly;
	activeWindows = pActiveWindows;
}

RenderEditor::~RenderEditor()
{
	ImGui::DestroyContext(context);
}