#pragma once

#include "Types.h"

class VertexAnim;

class MeshHandle
{
public:
	TYuint VAO = 0;
	TYuint Texture = 0;

	TYsizet indexCount = 0;
};

class Vertex
{
public:
	Vertex() :
		position({}), normal({}), texCoord({})
	{ }
	Vertex(TYvec pVertex, TYvec pNormal = TYvec(), TYvec2 pTexCoords = TYvec2()) :
		position(pVertex), normal(pNormal), texCoord(pTexCoords)
	{ }

	TYvec position;
	TYvec normal;
	TYvec2 texCoord;

	friend class VertexAnim;
};

class VertexAnim
{
public:
	VertexAnim() {}
	VertexAnim(TYvec pVertex, TYvec pNormal = TYvec(), TYvec2 pTexCoords = TYvec2()) :
		position(pVertex), normal(pNormal), texCoord(pTexCoords)
	{ }

	TYvec position;
	TYvec normal;
	TYvec2 texCoord;
	TYuvec4 boneIds = TYuvec4(0);
	TYvec4 boneWeights = TYvec4(0.0f);

	Vertex operator=(const VertexAnim& v)
	{
		return Vertex(position, normal, texCoord);
	}

	operator Vertex()
	{
		return Vertex(position, normal, texCoord);
	}

	friend class Vertex;
};