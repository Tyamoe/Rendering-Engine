#include "Debugger.h"

#include <algorithm>

#include "ImGuiUtils.h"

TYvoid Debug::Update(TYfloat dt)
{
	ImGui::Begin("Debug");

	ImGui::Text("Frame time %.3f ms", 1000.0f / ImGui::GetIO().Framerate);
	ImGui::Text("(%.1f FPS)", ImGui::GetIO().Framerate);
	ImGui::Text("(%.1f FPS2)", (1.0f / dt));

	ImGui::NewLine();

	ImGui::Text("Allocations: %d", Memory::allocs);
	ImGui::Text("Deallocations: %d", Memory::deallocs);
	ImGui::Text("Bytes in use: %zu", Memory::bytesInUse);

	ImGui::NewLine();

	ImGui::Text("Objects Drawn: %zu", objDrawn);
	ImGui::Text("Objects Culled: %zu", objSkipped);
	ImGui::Text("Lines Drawn: %zu", linesDrawn);
	ImGui::Text("Lines Culled: %zu", linesSkipped);

	ImGui::End();

	/*ImGui::Begin("Function Performance (ms)");

	std::for_each(systemsPerfInfo.begin(), systemsPerfInfo.end(),
		[&](auto pair)
	{
		ImGui::Text("%-24s", pair.first.c_str());
		ImGui::SameLine();
		ImGui::PushItemWidth(-300);
		ImGui::Text("%.4f", pair.second);
	});

	ImGui::End();*/

	//systemsPerfInfo.clear();
}