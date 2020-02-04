#include "stdafx.h"

#include "Window.h"
#include "Engine.h"
#include "Input.h"

#include "Structs.h"

int main()
{
	Window window(Settings(false, 0), Layout());

	EnginePtr engine = new Engine({ RayTrace, FlatColor, Editor });
	InputPtr input = new Input(window.GetGLFWWindow());

	window.AttachInput(input);
	window.AttachEngine(engine);
}