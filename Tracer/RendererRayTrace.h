#pragma once

#include "Types.h"

#include "Renderer.h"
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
};

typedef RenderRayTrace* RenderRayTracePtr;