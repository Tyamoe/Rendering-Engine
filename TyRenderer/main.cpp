#include "Engine.h"

int main()
{
	Engine* engine = new Engine();
	engine->Start();

	delete engine;
}