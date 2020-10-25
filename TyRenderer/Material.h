#pragma once

#include "Types.h"

enum class MaterialShader
{
	Phong,
	PBR,
};

class Material
{
public:
	Material(MaterialShader mS) : color({ 0.0f, 0.0f, 0.0f, 1.0f }), mShader(mS) {}
	Material() : color({0.0f, 0.0f, 0.0f, 1.0f}) {}
	Material(TYvec4 color_) : color(color_) {}
	~Material() {}

	TYfloat GetTransparency() { return 0.0f; }

	TYvec4 color;

	MaterialShader mShader = MaterialShader::Phong;

private:

public:
	static TYuint CreateTexture(TYstring filename);
};