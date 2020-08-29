#pragma once

#ifndef TYAMOE3D

#include "Types.h"

#else

#include "Tyamoe3DHelper.h"
#include EngineInc(Types.h)

#endif // TYAMOE3D

#include <algorithm>

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

	PixelColorF(TYvec v) :
		r(v.r), g(v.g), b(v.b), a(1.0f) {}

	operator TYvec()
	{
		return TYvec(r, g, b);
	}

	operator PixelColor()
	{
		return PixelColor(TYubyte(r * 255.0f), TYubyte(g * 255.0f), TYubyte(b * 255.0f), TYubyte(a * 255.0f));
	}

	TYbool operator==(const PixelColorF& a1) const
	{
		return r == a1.r && g == a1.g && b == a1.b;
	}

	PixelColorF operator+=(const PixelColorF& a1) const
	{
		return PixelColorF(r + a1.r, g + a1.g, b + a1.b, a);
	}

	PixelColorF operator-(const PixelColorF& a1) const
	{
		return PixelColorF(r - a1.r, g - a1.g, b - a1.b, a);
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

inline PixelColorF operator-(TYfloat f, const PixelColorF& p)
{
	return PixelColorF(f - p.r, f - p.g, f - p.b, p.a);
}

inline PixelColorF operator* (TYfloat f, const PixelColorF& p)
{
	return PixelColorF(p.r * f, p.g * f, p.b * f, p.a);
}

inline PixelColor::operator PixelColorF()
{
	return PixelColorF(r / 255.0f, b / 255.0f, g / 255.0f, a / 255.0f);
}
