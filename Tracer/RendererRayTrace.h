#pragma once

#include "Types.h"

#include "Renderer.h"
#include "Shader.h"
#include "Mesh.h"

class RenderRayTrace : public Renderer
{
	public:
		RenderRayTrace();
		~RenderRayTrace();

		TYvoid Init();

		TYvoid PreRender();
		TYvoid Render(TYfloat dt);
		TYvoid PostRender();

		TYuint AddMesh(Mesh& mesh);

	private:
		Mesh Scene;

		ShaderPtr RayTraceShader = TYnull;
		ShaderPtr QuadShader = TYnull;

		TYuint RenderBuffer;
		TYuint RenderTexture;
};

typedef RenderRayTrace* RenderRayTracePtr;