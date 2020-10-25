#pragma once

#include "Types.h"

class Window;
class Renderer;
struct ImGuiContext;

class TyRenderer
{
public:
	TyRenderer();

	TYvoid Update(TYfloat dt);

	friend class Engine;

private:
	Renderer* renderer = TYnull;
	Window* window = TYnull;
	ImGuiContext* imGuiContext = TYnull;

	TYfloat currDT = 0.0f;

	TYvoid PreRender();
	TYvoid Render();
	TYvoid PostRender();

public:
	static inline TYint width = 0;
	static inline TYint height = 0;
};