#pragma once

#include "Types.h"

#include "Renderer.h"
#include "Shader.h"
#include "Mesh.h"

#include "Scene.h"
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

		ScenePtr scene;
		CameraPtr camera;

	private:
		ShaderPtr QuadShader = TYnull;
		ShaderPtr BloomShader = TYnull;

		TYuint RenderBuffer;
		TYuint RenderTexture;

		TYuint Frame = 0;
};

typedef RenderRayTraceVulkan* RenderRayTracePtr;