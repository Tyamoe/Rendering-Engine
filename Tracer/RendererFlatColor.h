#pragma once

#include "Types.h"

#include "Renderer.h"
#include "Shader.h"
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
	ShaderPtr FlatShader = TYnull;
	ShaderPtr QuadShader = TYnull;

	TYuint RenderBuffer;
	TYuint RenderTexture;
};

typedef RenderFlatColor* RenderFlatColorPtr;