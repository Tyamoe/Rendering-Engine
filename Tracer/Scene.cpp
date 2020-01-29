#include "stdafx.h"

#include <filesystem>
#include "Scene.h"
#include "Render.h"

using namespace std::filesystem;

void CheckLoadModels()
{
	ImGui::Begin("Models");  // Model selection window


	const path location("./resources/models/");
	for (const auto& fileName : directory_iterator(location))
	{
		if (fileName.is_regular_file())
		{
			std::string button(fileName.path().filename().string());
			if (ImGui::Button(button.c_str()))
			{
				if (Models.size() == 9)
				{
					delete Models.back();
					Models.pop_back();  // Remove previous model
				}

				ModelPtr model = new Model("./resources/models/" + fileName.path().filename().string());
				Models.push_back(model);  // Load new model
			}
			ImGui::SameLine();
		}
	}

	ImGui::End();
}