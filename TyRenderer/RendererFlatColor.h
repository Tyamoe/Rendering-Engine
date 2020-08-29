#pragma once

#ifndef TYAMOE3D

#include "Types.h"
#include "Scene.h"

#else

#include "Tyamoe3DHelper.h"
#include EngineInc(Types.h)
#include EngineInc(Scene.h)

#endif // TYAMOE3D

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