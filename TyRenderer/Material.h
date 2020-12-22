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
	TYuint GetTexture(TYuint i) 
	{ 
		if (i < TextureList.size()) return TextureList[i];
		else return WhiteTexture;
	}

	TYvoid AddTexture(TYuint texture) { TextureList.push_back(texture); }

	TYvec4 color;

	MaterialShader mShader = MaterialShader::Phong;

private:
	TYvector<TYuint> TextureList;

public:
	static TYuint CreateTexture(TYstring& filename);
	static TYuint CreateTexture(TYvec4 color);

	static inline TYuint WhiteTexture = 0;

private:
	//static inline TYumap<TYvec4, TYuint> TextureMap;
};