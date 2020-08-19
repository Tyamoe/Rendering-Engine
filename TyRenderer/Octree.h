#pragma once

#include "Types.h"
#include "Utils.h"
#include "GenericDraw.h"

typedef class Triangle Triangle;
typedef class Geometry Geometry;

struct Triangle_
{
public:
	Triangle_(TYvec v0 = TYvec(0), TYvec v1 = TYvec(0), TYvec v2 = TYvec(0))
	{
		v[0] = v0;
		v[1] = v1;
		v[2] = v2;
	
		center = (v[0] + v[1] + v[2]) / 3.0f;

		TYvec min(std::numeric_limits<TYfloat>::max());
		TYvec max(-std::numeric_limits<TYfloat>::max());

		for (TYvec v1 : v)
		{
			if (v1.x > max.x) max.x = v1.x;
			if (v1.x < min.x) min.x = v1.x;
			if (v1.y > max.y) max.y = v1.y;
			if (v1.y < min.y) min.y = v1.y;
			if (v1.z > max.z) max.z = v1.z;
			if (v1.z < min.z) min.z = v1.z;
		}

		extents = max - min;
	}

	TYvec v[3];
	TYvec center;
	TYvec extents;
};

struct AABB
{
	TYvec min = TYvec(0.0f);
	TYvec max = TYvec(0.0f);
	TYvec mid = TYvec(0.0f);

	TYvoid Gen(TYvector<Triangle>& triangles);

	TYvoid Gen(TYvec pos, TYvec size)
	{
		min = pos;
		max = pos + size;
		mid = (max + min) / 2.0f;
	}
	
};

class Node
{
public:
	Node(AABB bound_, TYint depth_, TYvector<Triangle>& Triangles_) : bound(bound_), depth(depth_), Triangles(Triangles_)
	{
		for (TYint i = 0; i < 8; i++)
		{
			children[i] = TYnull;
		}
	}

	~Node()
	{

	}

	AABB bound;

	Node* children[8];

	TYint depth;

	TYvector<Triangle> Triangles;

private:

};

class Octree
{

public:
	TYvector3 culs;
	TYint  tempint = 0;

	TYfloat colorBias = 0.0f;

	TYfloat CulerDiff(TYvec cul)
	{
		TYfloat smol = 100.0f;
		for (TYint i = 0; i < tempint; i++)
		{
			TYfloat r = abs(cul.r - culs[i].r);
			TYfloat g = abs(cul.g - culs[i].g);
			TYfloat b = abs(cul.b - culs[i].b);

			if (r + g + b < smol)
			{
				smol = r + g + b;
			}
		}

		return smol;
	}

	TYvoid GenColors()
	{
		tempint = 0;
		for (TYint i = 0; i < 20; i++)
		{
			TYfloat r = GetRand(0.0f, 1.0f);
			TYfloat g = GetRand(0.0f, 1.0f);
			TYfloat b = GetRand(0.0f, 1.0f);
			TYvec c = TYvec(r, g, b);

			TYfloat d = CulerDiff(c);

			while (d < (colorBias))
			{
				r = GetRand(0.0f, 1.0f);
				g = GetRand(0.0f, 1.0f);
				b = GetRand(0.0f, 1.0f);
				c = TYvec(r, g, b);

				d = CulerDiff(c);
			}

			culs[i] = c;
			tempint++;
		}
	}

	Octree(Geometry* parentGeometry_) : parentGeometry(parentGeometry_)
	{
		maxDepth = 0;
		root = TYnull;
		colorBias = 0.05f;
		culs = TYvector3(20);
		GenColors();
	}

	~Octree()
	{
		//Delete(root);
	}

	TYvoid Draw(Node* node)
	{
		if (node == TYnull) return;

		TYvec color;

		if (node->depth != -1)
		{
			TYfloat width = 1.5f + (node->depth / 10.0f);
			color = culs[node->depth];
			GenericDraw::DrawCube(node->bound.mid, (node->bound.max - node->bound.min), color, width);

			for (int i = 0; i < 8; i++)
			{
				Draw(node->children[i]);
			}
		}
		else
		{
			if (drawEmpty)
			{
				color = TYvec(1.0f, 0.0f, 0.0f);
				GenericDraw::DrawCube(node->bound.mid, (node->bound.max - node->bound.min), color, 1.0f);
			}
		}
	}

	Node* Intersect(TYvec rayOrig, TYvec rayDir);

	TYbool TriInAABB(Triangle tri, AABB aabb);

	TYvoid MakeRec(Node* parent, TYint depth, TYvector<Triangle>& tries);

	TYvoid Break()
	{
		maxDepth = 0;
	}

	TYvoid Delete(Node* node)
	{
		if (node == TYnull) return;

		for (int i = 0; i < 8; i++)
		{
			Delete(node->children[i]);
			//delete node->children[i];
		}
		delete node;
	}

	TYvoid Make();

	TYvector<Triangle> GlobalTriangles;

	Node* root;

	TYint maxDepth = 0;

	TYbool done = false;
	TYbool drawEmpty = false;
	TYbool drawy = true;

	TYint  BranchCutoff = 300;

	Geometry* parentGeometry = TYnull;

private:

};
