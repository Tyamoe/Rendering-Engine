#include "stdafx.h"

#include "System.h"
#include "Render.h"
#include "Input.h"
#include "Scene.h"

GLFWwindow* window = nullptr;
bool running = true;
float dt = 0.0f;

int main()
{
	window = InitContext();

	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();
	ImGui::StyleColorsDark();

	InitRender();

	while (running && window && !glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		glClearColor(0.3f, 0.69f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		dt = ImGui::GetIO().DeltaTime;

		RenderModels();
		CheckLoadModels();

		running = !(Input::isKeyDown(GLFW_KEY_ESCAPE));

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		ImGui::EndFrame();

		glfwSwapBuffers(window);
		Input::Update(0);
	}

	Models.clear();
	CleanRender();

	ImGui_ImplOpenGL3_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();
}