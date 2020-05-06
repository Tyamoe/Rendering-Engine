#pragma once

#include "Types.h"

#include "Renderer.h"
#include "Shader.h"
#include "Mesh.h"

#include "Scene.h"
#include "Camera.h"
#include "Geometry.h"
#include "RenderingUtils.h"

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

		TYvoid RecompileRaytracer()
		{
			delete RayTraceShader;
			RayTraceShader = new Shader("raytracer.cs");
		}

		ScenePtr scene;
		CameraPtr camera;

	private:
		ShaderPtr RayTraceShader = TYnull;
		ShaderPtr QuadShader = TYnull;
		ShaderPtr BloomShader = TYnull;

		TYuint RenderBuffer;
		TYuint RenderTexture;

		TYuint Frame = 0;

		struct SPHERE
		{
			SPHERE(TYvec p, TYfloat r) : pos(p), radius(r)
			{

			}

			TYvec pos;
			TYfloat radius;
		};

		struct TRIANGLE
		{
			TRIANGLE(TYvec p0, TYvec p1, TYvec p2) : v0(p0), v1(p1), v2(p2)
			{
				c = (p0 + p1 + p2) / 3.0f;
			}

			TRIANGLE(TYvec p0, TYvec p1, TYvec p2, TYvec cc) : v0(p0), v1(p1), v2(p2), c(cc)
			{

			}

			TYvec v0;
			TYvec v1;
			TYvec v2;
			TYvec c;
		};

		struct SURFACE
		{
			SURFACE(TYvec s_, TYfloat t, TYvec e_, TYfloat r) : s(s_), trans(t), e(e_), ref(r)
			{
			}

			TYvec s;
			TYfloat trans;
			TYvec e;
			TYfloat ref;
		};

		TYint lightCount = 1;
		TYint sphereCount = 0;

		TYuint SSBO_Sphere = 0;
		TYuint SSBO_Triangle = 0;
		TYuint SSBO_Surface = 0;
};

typedef RenderRayTrace* RenderRayTracePtr;