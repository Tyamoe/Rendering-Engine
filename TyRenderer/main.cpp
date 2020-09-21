#include "stdafx.h"

#include "Window.h"
#include "Engine.h"
#include "Input.h"
#include "Globals.h"

#include "Structs.h"

#include "TyRenderer.h"

#ifdef HEADLESS

int main()
{
	/*Window window("Tray Racer", Settings(), Layout(800, 600, 400, 220)); // E

	//GenericDraw::Init(); // R

	EnginePtr engine = new Engine();
	InputPtr input = new Input(window.GetGLFWWindow()); // E

	window.AttachInput(input);
	window.AttachEngine(engine);

	TyRenderer tyRenderer; // E
	tyRenderer.Init();*/

	EnginePtr engine = new Engine();
	engine->Start();
}

#else

void Init()
{
	TYlog << "Init\n";
}

#endif