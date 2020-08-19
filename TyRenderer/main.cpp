#include "stdafx.h"

#include "Window.h"
#include "Engine.h"
#include "Input.h"
#include "Globals.h"

#include "Structs.h"
#include "GenericDraw.h"

#ifdef HEADLESS

int main()
{
	Window window("Tray Racer", Settings(), Layout(800, 600, 400, 220));

	GenericDraw::Init();

	EnginePtr engine = new Engine({ Editor });
	InputPtr input = new Input(window.GetGLFWWindow());

	window.AttachInput(input);
	window.AttachEngine(engine);
}

#else

void Init()
{
	TYlog << "Hey\n";
}

#endif