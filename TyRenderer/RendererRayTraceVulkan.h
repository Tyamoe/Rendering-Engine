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

#include "Camera.h"
#include "Geometry.h"
#include "RenderingUtils.h"

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
		ShaderPtr QuadShader = TYnull;
		ShaderPtr BloomShader = TYnull;

		TYuint RenderBuffer;
		TYuint RenderTexture;

		TYuint Frame = 0;
};

typedef RenderRayTraceVulkan* RenderRayTracePtr;