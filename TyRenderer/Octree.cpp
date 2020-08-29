#include "Tyamoe3DHelper.h"

#ifndef TYAMOE3D

#include "stdafx.h"

#else

#include EngineInc(stdafx.h)

#endif // TYAMOE3D

#include "Octree.h"
#include "Geometry.h"

TYvoid AABB::Gen(TYvector<Triangle>& triangles)
{
	TYvec mi(std::numeric_limits<TYfloat>::max());
	TYvec ma(-std::numeric_limits<TYfloat>::max());

	for (Triangle tri : triangles)
	{
		for (Vertex vert : tri.vertices.v)
		{
			TYvec v = vert.position;
			if (v.x > ma.x) ma.x = v.x;
			if (v.x < mi.x) mi.x = v.x;
			if (v.y > ma.y) ma.y = v.y;
			if (v.y < mi.y) mi.y = v.y;
			if (v.z > ma.z) ma.z = v.z;
			if (v.z < mi.z) mi.z = v.z;
		}
	}

	TYvec center = (ma + mi) / 2.0f;

	min = mi;
	max = ma;
	mid = center;
}

TYbool Intersect(TYvec rayOrig, TYvec rayDir, AABB aabb)
{
	TYfloat tmin = -TYmaxf;
	TYfloat tmax = TYmaxf;

	TYvec invD = 1.0f / rayDir;
	TYvec t0s = (aabb.min - rayOrig) * invD;
	TYvec t1s = (aabb.max - rayOrig) * invD;

	TYvec tsmaller = min(t0s, t1s);
	TYvec tbigger = max(t0s, t1s);

	tmin = glm::max(tmin, glm::max(tsmaller[0], glm::max(tsmaller[1], tsmaller[2])));
	tmax = glm::min(tmax, glm::min(tbigger[0], glm::min(tbigger[1], tbigger[2])));

	return (tmin < tmax);
}

Node* Octree::Intersect(TYvec rayOrig, TYvec rayDir)
{
	Node* node = TYnull;

	if(::Intersect(rayOrig, rayDir, root->bound))
	{
		node = root;

		TYfloat minDis = TYmaxf;
		//return root;
		for (TYint i = 0; i < 8; i++)
		{
			if (!root->children[i]) continue;
			if(::Intersect(rayOrig, rayDir, root->children[i]->bound))
			{
				TYfloat dis = glm::distance(rayOrig, root->children[i]->bound.mid);
				if (dis < minDis)
				{
					minDis = dis;
					node = root->children[i];
				}
			}
		}
		/*minDis = TYmaxf;
		Node* tnode = node;
		for (TYint i = 0; i < 8; i++)
		{
			if (!node->children[i]) continue;
			if (::Intersect(rayOrig, rayDir, node->children[i]->bound))
			{
				TYfloat dis = glm::distance(rayOrig, node->children[i]->bound.mid);
				if (dis < minDis)
				{
					minDis = dis;
					tnode = node->children[i];
				}
			}
		}
		node = tnode;*/
	}

	return node;
}

TYbool Octree::TriInAABB(Triangle tri, AABB aabb)
{
	TYvec half = (aabb.max - aabb.min) * 0.5f;
	TYint in = 0;
	for (TYint i = 0; i < 3; i++)
	{
		TYvec d = tri.vertices[i].position - aabb.mid;
		TYbool inside = (abs(glm::dot(d, TYvec(1, 0, 0))) <= half.x) &&
						(abs(glm::dot(d, TYvec(0, 1, 0))) <= half.y) &&
						(abs(glm::dot(d, TYvec(0, 0, 1))) <= half.z);

		if (inside)
		{
			in++;
			//return false;
		}
	}
	return in >= 0;
}

TYvoid Octree::MakeRec(Node* parent, TYint depth, TYvector<Triangle>& tries)
{
	TYvec dim = (parent->bound.max - parent->bound.min) * 0.5f;
	depth++;

	for (int i = 0; i < 8; i++)
	{
		TYint childTrisCount = 0;
		TYvector<Triangle> childTris;
		AABB childAABB;

		switch (i)
		{
		case 0:
			childAABB.Gen(parent->bound.min, dim);
			break;
		case 1:
			childAABB.Gen(parent->bound.min + TYvec(dim.x, 0, 0), dim);
			break;
		case 2:
			childAABB.Gen(parent->bound.min + TYvec(0, 0, dim.z), dim);
			break;
		case 3:
			childAABB.Gen(parent->bound.min + TYvec(dim.x, 0, dim.z), dim);
			break;
		case 4:
			childAABB.Gen(parent->bound.min + TYvec(0, dim.y, 0), dim);
			break;
		case 5:
			childAABB.Gen(parent->bound.min + TYvec(dim.x, dim.y, 0), dim);
			break;
		case 6:
			childAABB.Gen(parent->bound.min + TYvec(0, dim.y, dim.z), dim);
			break;
		case 7:
			childAABB.Gen(parent->bound.min + TYvec(dim.x, dim.y, dim.z), dim);
			break;
		}

		for (TYsizet j = 0; j < tries.size(); j++)
		{
			if (TriInAABB(tries[j], childAABB))
			{
				childTris.push_back(tries[j]);
				childTrisCount++;
			}
		}

		if (childTrisCount == 0)
		{
			return;
		}
		if (depth > 2)
		{
			Node* child = new Node(childAABB, -1, childTris);
			parent->children[i] = child;
			//TYlog << depth << " stopped" << TYlogbreak;
			maxDepth = depth > maxDepth ? depth : maxDepth;
			//return;
		}
		else
		{
			Node* child = new Node(childAABB, depth, childTris);
			parent->children[i] = child;
			MakeRec(child, depth, childTris);
		}
	}
}

TYvoid Octree::Make()
{
	TYsizet count = GlobalTriangles.size();

	AABB aabb;
	aabb.Gen(GlobalTriangles);

	root = new Node(aabb, 0, GlobalTriangles);

	MakeRec(root, 0, GlobalTriangles);
	done = true;
}
