#pragma once

#include "Types.h"
#include "Utils.h"

class Triangle;
class Geometry;

struct Plane
{
	Plane() {}
	Plane(TYfloat c, TYvec orientation, TYvec pos) : Distance(c), Center(pos), Normal(orientation)
	{

	}

	TYvec Center;
	TYvec Normal;
	TYfloat Distance;

	TYfloat RayDistance(TYvec rayOrig, TYvec rayDir) // Gives the distance along the ray (+direction) where intersection with the plane occurs
	{
		TYfloat dis = TYinf;
		if(!Intersect(rayOrig, rayDir, dis))
			dis = TYinf;;
		return dis;
	}

	TYbool Intersect(TYvec rayOrig, TYvec rayDir, TYfloat& t)
	{
		TYfloat denom = glm::dot(Normal, rayDir);
		if (abs(denom) > TYepsilon)
		{
			t = glm::dot((Center - rayOrig), Normal) / denom;
			if (t >= 0) return true;
		}
		return false;
	}
};

struct AABB
{
	AABB() {}
	TYvec min = TYvec(0.0f);
	TYvec max = TYvec(0.0f);
	TYvec mid = TYvec(0.0f);

	Plane midPlanes[3];

	TYvoid Planify()
	{
		midPlanes[0] = Plane(mid.z, TYvec(0, 0, 1), mid);
		midPlanes[1] = Plane(mid.y, TYvec(0, 1, 0), mid);
		midPlanes[2] = Plane(mid.x, TYvec(1, 0, 0), mid);
	}

	TYvoid Gen(TYvector<Triangle>& triangles);

	TYvoid Gen(TYvec pos, TYvec size)
	{
		min = pos;
		max = pos + size;
		mid = (max + min) / 2.0f;
	}

	TYbool TriInAABB(Triangle& tri);
	TYbool PointInAABB(TYvec point);

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
		//if (Triangles_.size() > 0) empty = true;
	}
	Node(AABB bound_, TYint depth_) : bound(bound_), depth(depth_)
	{
		for (TYint i = 0; i < 8; i++)
		{
			children[i] = TYnull;
		}
	}

	~Node()
	{
		delete color;
	}

	struct HitInfo
	{
		TYint hitIndex = -1;
		TYfloat t0 = TYmaxf;
		TYfloat t1 = TYmaxf;
		TYvec normal = {};
	};

	HitInfo hitInfo;

	AABB bound;

	Node* children[8];

	TYbool empty = false;

	TYint depth;

	TYvec* color = TYnull;

	TYvector<Triangle> Triangles;

	TYint childID = 0;

	Node* Intersect(const TYvec& rayOrig, const TYvec& rayDir, TYvec hitPoint, TYmat& modelMat);

private:
	TYint SignMask(TYvec vec);
	TYint SignMask(TYfloat f, TYfloat c);

};

class Octree
{
public:
	Octree(Geometry* parentGeometry_);

	~Octree()
	{
		Delete(root);
	}

	TYvoid Delete(Node* node)
	{
		if (node == TYnull) return;

		for (int i = 0; i < 8; i++)
		{
			Delete(node->children[i]);
		}
		delete node;
	}

	TYvoid Make(TYvector<Triangle>& GlobalTriangles);
	TYvoid MakeRec(Node* parent, TYint depth, TYvector<Triangle>& tries);

	TYvoid GenColors();

	TYvoid Traverse(Node* node);

	TYvoid Draw(Node* node);

	Node* Intersect(TYvec rayOrig, TYvec rayDir, TYmat& modelMat);

	Node* root;

	TYint maxDepth = 0;

	TYbool done = false;
	TYbool drawEmpty = true;
	TYbool drawy = true;

	TYint BranchCutoff = 300;

	TYvector3 colors;
	TYint tempint = 0;

	TYfloat colorBias = 0.0f;

	Geometry* parentGeometry = TYnull;

private:
	TYfloat ColorDiff(TYvec cul);

};

enum class Octant
{
	Fr_BL = 1,
	Fr_BR = 5,
	Fr_TL = 3,
	Fr_TR = 7,

	Ba_BL = 0,
	Ba_BR = 4,
	Ba_TL = 2,
	Ba_TR = 6,

	Root = 8
};

static const char* OctantStrings[] = { "Back_BottomL", "Front_BottomL", "Back_TopL", "Front_TopL",
									   "Back_BottomR", "Front_BottomR", "Back_TopR", "Front_TopR"
										,"Root" };
