#pragma once

#ifndef TYAMOE3D

#include "Types.h"
#include "Utils.h"

#else

#include "Tyamoe3DHelper.h"
#include EngineInc(Types.h)
#include EngineInc(Utils.h)

#endif // TYAMOE3D

typedef class Triangle Triangle;
typedef class Geometry Geometry;

class Plane
{
public:
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

class Segment;

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
public:
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

	TYbool IntersectLineAABB(TYvec O, TYvec D, TYfloat t[], TYfloat epsilon);

	TYbool SegmentIntersectsGridAlignedBox3D(Segment segment);

};



class Segment
{
public:
	Segment(const TYvec& startPoint, const TYvec& endPoint) :
		origin(startPoint), direction(endPoint - startPoint),
		inverseDirection(TYvec(1.0f) / direction),
		sign{ (inverseDirection.x < 0.0f),(inverseDirection.y < 0.0f),(inverseDirection.z < 0.0f) }
	{}

	TYfloat length() {
		return sqrtf(direction.x * direction.x + direction.y * direction.y +
			direction.z * direction.z);
	}
	TYvec origin, endpoint, direction;
	TYvec inverseDirection;
	TYint sign[3];
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

		if (Triangles_.size() > 0) empty = true;
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

	}

	AABB bound;

	Node* children[8];

	TYbool empty = false;

	TYint depth;

	TYvec* color = TYnull;

	TYvector<Triangle> Triangles;

	TYint childID = 0;

	Node* Intersect(TYvec rayOrig, TYvec rayDir);

private:

};

class Octree
{

public:
	TYvector3 colors;
	TYint  tempint = 0;

	TYfloat colorBias = 0.0f;

	TYfloat ColorDiff(TYvec cul);

	TYvoid GenColors();

	Octree(Geometry* parentGeometry_);

	~Octree()
	{
		//Delete(root);
	}

	TYvoid Traverse(Node* node);

	TYvoid Draw(Node* node);

	Node* Intersect(TYvec rayOrig, TYvec rayDir);

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
