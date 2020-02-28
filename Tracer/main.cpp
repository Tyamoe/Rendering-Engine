#include "stdafx.h"

#include "Window.h"
#include "Engine.h"
#include "Input.h"
#include "Globals.h"

#include "Structs.h"

int main()
{
	Window window("Tray Racer", Settings(), Layout(400, 400, 400, 300));

	EnginePtr engine = new Engine({ RayTraceCPU, Editor });
	InputPtr input = new Input(window.GetGLFWWindow());

	window.AttachInput(input);
	window.AttachEngine(engine);
}