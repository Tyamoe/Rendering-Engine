#include "stdafx.h"

#include <filesystem>

#include "RendererEditor.h"

using namespace std::filesystem;

TYvoid RenderEditor::PreRender()
{
	/*ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();*/
}

TYvoid RenderEditor::Render(TYfloat dt)
{
	/*ImGui::Begin("Model Browser");

	const path location("./resources/models/");
	for (const auto& fileName : directory_iterator(location))
	{
		if (fileName.is_regular_file())
		{
			TYstring button(fileName.path().filename().string());
			if (ImGui::Button(button.c_str()))
			{
				TYlog << button << TYlogbreak;
			}
			ImGui::SameLine();
		}
	}

	ImGui::End();*/
}

TYvoid RenderEditor::PostRender()
{
	/*ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	ImGui::EndFrame();*/
}

TYvoid RenderEditor::Init()
{
	context = ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(engine->GetGLFWWindow(), false);
	ImGui_ImplOpenGL3_Init();
	ImGui::StyleColorsDark();
}

RenderEditor::RenderEditor(TYbool pReadonly, Windows pActiveWindows)
{
	readonly = pReadonly;
	activeWindows = pActiveWindows;
}

RenderEditor::~RenderEditor()
{
	ImGui::DestroyContext(context);
}