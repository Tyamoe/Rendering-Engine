#include "stdafx.h"

#include "Window.h"
#include "Engine.h"
#include "Input.h"

#include "Structs.h"

int main()
{
	Window window(Settings(false, 0), Layout(500, 500, 200, 100));

	EnginePtr engine = new Engine({ RayTraceCPU, Editor });
	InputPtr input = new Input(window.GetGLFWWindow());

	/*while (!glfwWindowShouldClose(window.GetGLFWWindow()))
	{
		glfwPollEvents();

		glViewport(0, 0, 500, 500);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0.3f, 0.69f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		glfwSwapBuffers(window.GetGLFWWindow());
	}*/

	window.AttachInput(input);
	window.AttachEngine(engine);
}