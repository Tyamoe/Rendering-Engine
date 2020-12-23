#include "Octree.h"

#include "Geometry.h"
#include "GenericDraw.h"
#include "Debugger.h"

Octree::Octree(Geometry* parentGeometry_) : parentGeometry(parentGeometry_)
{
	maxDepth = 0;
	root = TYnull;
	colorBias = 0.05f;
	colors = TYvector3(20);
	GenColors();
}

TYvoid Octree::Make(TYvector<Triangle>& GlobalTriangles)
{
	TYsizet count = GlobalTriangles.size();

	AABB aabb;
	aabb.Gen(GlobalTriangles);
	aabb.Planify();

	root = new Node(aabb, 0);
	root->childID = 8;

	MakeRec(root, 0, GlobalTriangles);
	done = true;
}

TYvoid Octree::MakeRec(Node* parent, TYint depth, TYvector<Triangle>& tries)
{
	TYvec dim = (parent->bound.max - parent->bound.min) * 0.5f;
	depth++;

	for (TYint i = 0; i < 8; i++)
	{
		TYint childTrisCount = 0;
		TYvector<Triangle> childTris;
		AABB childAABB;

		switch (i)
		{
			// Front - Bottom
		case 1: // Left
			childAABB.Gen(parent->bound.min + TYvec(0, 0, dim.z), dim);
			break;
		case 5: // Right
			childAABB.Gen(parent->bound.min + TYvec(dim.x, 0, dim.z), dim);
			break;

			// Front - Top
		case 3: // Left
			childAABB.Gen(parent->bound.min + TYvec(0, dim.y, dim.z), dim);
			break;
		case 7: // Right
			childAABB.Gen(parent->bound.min + TYvec(dim.x, dim.y, dim.z), dim);
			break;

			// Back - Bottom
		case 0: // Left
			childAABB.Gen(parent->bound.min, dim);
			break;
		case 4: // Right
			childAABB.Gen(parent->bound.min + TYvec(dim.x, 0, 0), dim);
			break;

			// Back - Top
		case 2: // Left
			childAABB.Gen(parent->bound.min + TYvec(0, dim.y, 0), dim);
			break;
		case 6: // Right
			childAABB.Gen(parent->bound.min + TYvec(dim.x, dim.y, 0), dim);
			break;
		}
		childAABB.Planify();

		for (TYsizet j = 0; j < tries.size(); j++)
		{
			if (childAABB.TriInAABB(tries[j]))
			{
				childTris.push_back(tries[j]);
				childTrisCount++;
			}
		}

		if (childTrisCount == 0)
		{
			//return;
		}
		else
		{
			if (childTrisCount <= 10 || depth >= 2)
			{
				//Node* child = new Node(childAABB, -1, childTris);
				Node* child = new Node(childAABB, -1, childTris);
				child->childID = i;
				parent->children[i] = child;

				//TYlog << depth << " stopped" << TYlogbreak;
				maxDepth = depth > maxDepth ? depth : maxDepth;
				//return;
			}
			else
			{
				//Node* child = new Node(childAABB, depth, childTris);
				Node* child = new Node(childAABB, depth);
				child->childID = i;
				parent->children[i] = child;
				MakeRec(child, depth, childTris);
			}
		}
	}
}

TYvoid AABB::Gen(TYvector<Triangle>& triangles)
{
	TYvec minCoord(std::numeric_limits<TYfloat>::max());
	TYvec maxCoord(-std::numeric_limits<TYfloat>::max());

	for (Triangle& tri : triangles)
	{
		for (Vertex& vert : tri.vertices.v)
		{
			TYvec v = vert.position;
			if (v.x > maxCoord.x) maxCoord.x = v.x;
			if (v.x < minCoord.x) minCoord.x = v.x;

			if (v.y > maxCoord.y) maxCoord.y = v.y;
			if (v.y < minCoord.y) minCoord.y = v.y;

			if (v.z > maxCoord.z) maxCoord.z = v.z;
			if (v.z < minCoord.z) minCoord.z = v.z;
		}
	}

	TYvec center = (maxCoord + minCoord) / 2.0f;

	min = minCoord;
	max = maxCoord;
	mid = center;
}

TYbool AABB::TriInAABB(Triangle& tri)
{
	/*TYvec half = (max - min) * 0.5f;

	//return intersects_box(tri, mid, (max - min) * 0.5f);

	if (SegmentIntersectsGridAlignedBox3D(Segment(tri.vertices[0].position, tri.vertices[1].position)))
	{
		return true;
	}
	if (SegmentIntersectsGridAlignedBox3D(Segment(tri.vertices[0].position, tri.vertices[2].position)))
	{
		return true;
	}
	if (SegmentIntersectsGridAlignedBox3D(Segment(tri.vertices[1].position, tri.vertices[2].position)))
	{
		return true;
	}

	return false;*/

	TYint in = 0;
	for (TYint i = 0; i < 3; i++)
	{
		/*TYvec d = tri.vertices[i].position - mid;
		TYbool inside = (abs(glm::dot(d, TYvec(1, 0, 0))) <= half.x) &&
			(abs(glm::dot(d, TYvec(0, 1, 0))) <= half.y) &&
			(abs(glm::dot(d, TYvec(0, 0, 1))) <= half.z);

		if (inside)
		{
			in++;
			//return false;
		}*/

		if (PointInAABB(tri.vertices[i].position))
		{
			return true;
			in++;
		}
	}
	return in >= 1;
	return true;
}

TYbool AABB::PointInAABB(TYvec point)
{
	TYvec half = (max - min) ;

	TYvec d = point - mid;
	TYbool inside = (abs(glm::dot(d, TYvec(1, 0, 0))) <= half.x) &&
		(abs(glm::dot(d, TYvec(0, 1, 0))) <= half.y) &&
		(abs(glm::dot(d, TYvec(0, 0, 1))) <= half.z);

	return inside;
}

TYvoid Octree::Draw(Node* node)
{
	if (node == TYnull) return;

	TYvec color;

	GenericDraw::DrawText(std::to_string(node->Triangles.size()), node->bound.mid, (0.0010f * 0.25f), TYvec4(1.0f, 0.0f, 0.0f, 1.0f));
	if (node->depth != -1)
	{
		TYfloat width = 1.1f - (node->childID / 10.0f);

		if (node->childID == 8) width = 2.0f;

		if (node->color == TYnull)
		{
			color = colors[node->childID];
		}
		else
		{
			color = *node->color;
			//delete node->color;
			//node->color = TYnull;
		}

		//color = colors[node->childID];
		GenericDraw::DrawSphere(node->bound.mid, 0.04f, color);
		GenericDraw::DrawCube(node->bound.mid, (node->bound.max - node->bound.min), color, width);

		for (TYint i = 0; i < 8; i++)
		{
			Draw(node->children[i]);
		}
	}
	else
	{
		if (drawEmpty)
		{
			if (node->color != TYnull)
			{
				color = *node->color;
			}
			else
			{
				color = colors[node->childID];
			}

			GenericDraw::DrawSphere(node->bound.mid, 0.04f, color);
			if (node->color != TYnull)
			{
				color = *node->color;
			}
			else
			{
				color = TYvec(1.0f, 1.0f, 1.0f);
			}
			GenericDraw::DrawCube(node->bound.mid, (node->bound.max - node->bound.min), color, 1.0f);
		}
	}
}

TYvoid Octree::Traverse(Node* node)
{
	if (node == TYnull) return;

	//TYlog << "Node: d: " << node->depth << " c: " << node->childID
	//	<< " t: " << node->Triangles.size() << " e: " << node->empty << "\n";

	printf("Node: d: %d | c: %d | t: %d | e: %d \n", node->depth, node->childID, (TYint)node->Triangles.size(), node->empty);

	if (node->depth != -1)
	{
		for (TYint i = 0; i < 8; i++)
		{
			Traverse(node->children[i]);
		}
	}
	else
	{

	}
}

TYvoid Octree::GenColors()
{
	colors[0] = TYvec(1.0f, 0.0f, 0.0f);
	colors[1] = TYvec(0.0f, 1.0f, 0.0f);
	colors[2] = TYvec(0.0f, 0.0f, 1.0f);
	colors[3] = TYvec(1.0f, 1.0f, 0.0f);

	colors[4] = TYvec(0.5f, 0.0f, 0.0f);
	colors[5] = TYvec(0.0f, 0.5f, 0.0f);
	colors[6] = TYvec(0.0f, 0.0f, 0.5f);
	colors[7] = TYvec(0.5f, 0.5f, 0.0f);

	colors[8] = TYvec(0.0f, 0.0f, 0.0f);

	tempint = 0;
	for (TYint i = 9; i < 20; i++)
	{
		TYfloat r = GetRand(0.0f, 1.0f);
		TYfloat g = GetRand(0.0f, 1.0f);
		TYfloat b = GetRand(0.0f, 1.0f);
		TYvec c = TYvec(r, g, b);

		TYfloat d = ColorDiff(c);

		while (d < (colorBias))
		{
			r = GetRand(0.0f, 1.0f);
			g = GetRand(0.0f, 1.0f);
			b = GetRand(0.0f, 1.0f);
			c = TYvec(r, g, b);

			d = ColorDiff(c);
		}

		colors[i] = c;
		tempint++;
	}
}

TYfloat Octree::ColorDiff(TYvec cul)
{
	TYfloat smol = 100.0f;
	for (TYint i = 0; i < tempint; i++)
	{
		TYfloat r = abs(cul.r - colors[i].r);
		TYfloat g = abs(cul.g - colors[i].g);
		TYfloat b = abs(cul.b - colors[i].b);

		if (r + g + b < smol)
		{
			smol = r + g + b;
		}
	}

	return smol;
}

TYbool Intersect(TYvec rayOrig, TYvec rayDir, const AABB& aabb, TYfloat& t)
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

	t = tmin;

	return (tmin < tmax);
}

Node* Octree::Intersect(TYvec rayOrig, TYvec rayDir)
{
	Node* node = TYnull;
	TYfloat t = 0.0f;
	if (::Intersect(rayOrig, rayDir, root->bound, t))
	{
		TYvec hitPoint = rayOrig + rayDir * t;
		node = root->Intersect(rayOrig, rayDir, hitPoint);
	}
	return node;
}

Node* Node::Intersect(const TYvec& rayOrig, const TYvec& rayDir, TYvec hitPoint)
{
	if (Triangles.size() > 0)
	{
		return this;
	}

	TYfloat xDist = glm::distance2(hitPoint, bound.midPlanes[2].Center * bound.midPlanes[2].Normal);
	TYfloat yDist = glm::distance2(hitPoint, bound.midPlanes[1].Center * bound.midPlanes[1].Normal);
	TYfloat zDist = glm::distance2(hitPoint, bound.midPlanes[0].Center * bound.midPlanes[0].Normal);

	TYint lastFlip = -1;
	for (TYint i = 0; i < 3; i++)
	{
		TYint index = SignMask(hitPoint);

		Node* ret = TYnull;
		if (children[index])
		{
			ret = children[index]->Intersect(rayOrig, rayDir, hitPoint);
		}

		if (ret != TYnull)
		{
			return ret;
		}

		TYfloat minDist = glm::min(glm::min(xDist, yDist), zDist);
		if (minDist == TYinf) return TYnull;

		if (lastFlip != -1)
		{
			hitPoint[lastFlip] = -hitPoint[lastFlip];
			lastFlip = -1;
		}

		if (minDist == xDist)
		{
			xDist = TYinf;
			hitPoint.x = -hitPoint.x;
			lastFlip = 0;
		}
		else if (minDist == yDist)
		{
			yDist = TYinf;
			hitPoint.y = -hitPoint.y;
			lastFlip = 1;
		}
		else if (minDist == zDist)
		{
			zDist = TYinf;
			hitPoint.z = -hitPoint.z;
			lastFlip = 2;
		}
	}
	return TYnull;
}

TYint Node::SignMask(TYvec vec)
{
	return SignMask(vec.z, bound.mid.z) | (SignMask(vec.y, bound.mid.y) << 1) | (SignMask(vec.x, bound.mid.x) << 2);
}

TYint Node::SignMask(TYfloat f, TYfloat c)
{
	return f > c;
}
