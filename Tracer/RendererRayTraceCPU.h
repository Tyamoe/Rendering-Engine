#pragma once

#include "Types.h"

#include "Renderer.h"
#include "Shader.h"
#include "Mesh.h"

class RenderRayTraceCPU : public Renderer
{
	public:
		RenderRayTraceCPU();
		~RenderRayTraceCPU();

		TYvoid Init();

		TYvoid PreRender();
		TYvoid Render(TYfloat dt);
		TYvoid PostRender();

		TYuint AddMesh(Mesh& mesh);

		TYvoid RandomBS();

	private:
		Mesh Scene;

		ShaderPtr QuadShader = TYnull;

		TYvector3 PixelBuffer;

		TYuint RenderBuffer;
		TYuint RenderTexture;
};

typedef RenderRayTraceCPU* RenderRayTraceCPUPtr;