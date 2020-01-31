#pragma once

#include "Types.h"

#include "Renderer.h"
#include "Mesh.h"

class RenderFlatColor : public Renderer
{
public:
	RenderFlatColor();
	~RenderFlatColor();

	TYvoid Init();

	TYvoid PreRender();
	TYvoid Render(TYfloat dt);
	TYvoid PostRender();

private:
};

typedef RenderFlatColor* RenderFlatColorPtr;