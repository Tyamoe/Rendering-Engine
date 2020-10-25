#pragma once

#include "Types.h"

#include "Renderer.h"
#include "RenderingUtils.h"

class Shader;

class RenderRayTraceVulkan : public Renderer
{
	public:
		RenderRayTraceVulkan();
		~RenderRayTraceVulkan();

		TYvoid Init();

		TYvoid PreRender();
		TYvoid Render(TYfloat dt);
		TYvoid PostRender();

	private:
		Shader* QuadShader = TYnull;
		Shader* BloomShader = TYnull;

		TYuint RenderBuffer;
		TYuint RenderTexture;

		TYuint Frame = 0;
};

typedef RenderRayTraceVulkan* RenderRayTracePtr;