#pragma once

#include <algorithm>

#include "Types.h"

#include "Renderer.h"
#include "Shader.h"
#include "Mesh.h"

union PixelColorF;

union PixelColor
{
	PixelColor() : r(0), g(0), b(0), a(255) {}

	PixelColor(TYubyte red, TYubyte green, TYubyte blue, TYubyte alpha) :
		r(red), g(green), b(blue), a(alpha) {}

	PixelColor(TYubyte red, TYubyte green, TYubyte blue) :
		r(red), g(green), b(blue), a(255) {}

	PixelColor(TYubyte all)
	{
		r = g = b = all;
		a = 255;
	}

	operator PixelColorF();

	PixelColor operator+=(const PixelColor& a1) const
	{
		return PixelColor(r + a1.r, g + a1.g, b + a1.b, a);
	}

	PixelColor operator+(const PixelColor& a1) const
	{
		return PixelColor(r + a1.r, g + a1.g, b + a1.b, a);
	}

	PixelColor operator*(const float& a1) const
	{
		TYvec c1 = TYvec(r / 255.0f, b / 255.0f, g / 255.0f);
		c1 *= a1;
		return PixelColor(TYubyte(c1.r * a1), TYubyte(c1.g * a1), TYubyte(c1.b * a1), a);
	}

	TYvoid Clamp()
	{
		r = std::min(TYubyte(255), r);
		g = std::min(TYubyte(255), g);
		b = std::min(TYubyte(255), b);
	}

	struct
	{
		TYubyte r, g, b, a;
	};

	TYubyte val[4];

	GLuint raw;
};

union PixelColorF
{
	PixelColorF() : r(0.0f), g(0.0f), b(0.0f), a(1.0f) {}

	PixelColorF(TYfloat red, TYfloat green, TYfloat blue, TYfloat alpha) :
		r(red), g(green), b(blue), a(alpha) {}

	PixelColorF(TYfloat red, TYfloat green, TYfloat blue) :
		r(red), g(green), b(blue), a(1.0f) {}

	PixelColorF(TYfloat all)
	{
		r = g = b = all;
		a = 1.0f;
	}

	operator PixelColor()
	{
		return PixelColor(TYubyte(r * 255.0f), TYubyte(g * 255.0f), TYubyte(b * 255.0f), TYubyte(a * 255.0f));
	}

	PixelColorF operator+=(const PixelColorF& a1) const
	{
		return PixelColorF(r + a1.r, g + a1.g, b + a1.b, a);
	}

	PixelColorF operator+(const PixelColorF& a1) const
	{
		return PixelColorF(r + a1.r, g + a1.g, b + a1.b, a);
	}

	PixelColorF operator*(const PixelColorF& a1) const
	{
		return PixelColorF(r * a1.r, g * a1.g, b * a1.b, a);
	}

	PixelColorF operator*(const TYfloat& a1) const
	{
		return PixelColorF(r * a1, g * a1, b * a1, a);
	}

	TYvoid Clamp()
	{
		r = std::min(1.0f, r);
		g = std::min(1.0f, g);
		b = std::min(1.0f, b);
		a = std::min(1.0f, a);
	}

	struct
	{
		TYfloat r;
		TYfloat g;
		TYfloat b;
		TYfloat a;
	};

	TYubyte raw[sizeof(TYfloat) * 4];
};

inline PixelColor::operator PixelColorF()
{
	return PixelColorF(r / 255.0f, b / 255.0f, g / 255.0f, a / 255.0f);
}

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

	private:
		Mesh Scene;

		ShaderPtr QuadShader = TYnull;

		TYuint RenderBuffer = 0;
		TYuint RenderTexture = 0;

		TYuint Frame = 0;

		TYuint PBO = 0;
		PixelColor* PixelBuffer = TYnull;
		PixelColor* ImageBuffer = TYnull;

		PixelColor clearColor = PixelColor(89, 155, 100, 255);
		TYint pixelCount = 0;

		TYvoid TraceRays();
		PixelColorF Trace(TYvec rayOrigin, TYvec rayDir, TYint rayDepth);
};

typedef RenderRayTraceCPU* RenderRayTraceCPUPtr;