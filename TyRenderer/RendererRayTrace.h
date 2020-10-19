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

		TYvoid RecompileRaytracer()
		{
			delete RayTraceShader;
			RayTraceShader = new Shader("raytracer.cs");
		}

	private:
		ShaderPtr RayTraceShader = TYnull;
		ShaderPtr QuadShader = TYnull;
		ShaderPtr BloomShader = TYnull;

		TYuint Frame = 0;

		TYuint RenderBuffer;
		TYuint RenderTexture;

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
				pad1 = 0.0f;
				pad2 = 0.0f;
				pad3 = 0.0f;
			}

			TRIANGLE(TYvec p0, TYvec p1, TYvec p2, TYvec cc) : v0(p0), v1(p1), v2(p2)
			{
				pad1 = 0.0f;
				pad2 = 0.0f;
				pad3 = 0.0f;
			}

			TYvec v0;
			TYfloat pad1;
			TYvec v1;
			TYfloat pad2;
			TYvec v2;
			TYfloat pad3;
		};

		struct MODEL
		{
			MODEL(TYint o, TYint t) : offset(o), triCount(t)
			{
			}

			TYint offset;
			TYint triCount;
		};

		struct BOUND
		{
			BOUND(TYvec p, TYfloat r) : pos(p), radius(r)
			{
			}
			TYvec pos;
			TYfloat radius;
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
		TYuint SSBO_Model = 0;
		TYuint SSBO_Bound = 0;
		TYuint SSBO_Surface = 0;
};

typedef RenderRayTrace* RenderRayTracePtr;