#include "Octree.h"

#include "Geometry.h"
#include "GenericDraw.h"
#include "Debugger.h"

///////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////
TYbool intersects_box(Triangle triangle, TYvec box_center, TYvec box_extents)
{
	const TYint X = 0, Y = 1, Z = 2;

	// Translate triangle as conceptually moving AABB to origin
	TYvec v0 = triangle.vertices[0].position - box_center;
	TYvec v1 = triangle.vertices[1].position - box_center;
	TYvec v2 = triangle.vertices[2].position - box_center;

	// Compute edge vectors for triangle
	TYvec f0 = triangle.vertices[1].position - triangle.vertices[0].position;
	TYvec f1 = triangle.vertices[2].position - triangle.vertices[1].position;
	TYvec f2 = triangle.vertices[0].position - triangle.vertices[2].position;

//// region Test axes a00..a22 (category 3)

	// Test axis a00
	TYvec a00 = TYvec(0, -f0[Z], f0[Y]);
	TYfloat p0 = glm::dot(v0, a00);
	TYfloat p1 = glm::dot(v1, a00);
	TYfloat p2 = glm::dot(v2, a00);
	TYfloat r = box_extents[Y] * abs(f0[Z]) + box_extents[Z] * abs(f0[Y]);
	if (glm::max(-glm::max(glm::max(p0, p1), p2), glm::min(glm::min(p0, p1), p2)) > r)
		return false;

	// Test axis a01
	TYvec a01 = TYvec(0, -f1[Z], f1[Y]);
	p0 = glm::dot(v0, a01);
	p1 = glm::dot(v1, a01);
	p2 = glm::dot(v2, a01);
	r = box_extents[Y] * abs(f1[Z]) + box_extents[Z] * abs(f1[Y]);
	if (glm::max(-glm::max(glm::max(p0, p1), p2), glm::min(glm::min(p0, p1), p2)) > r)
		return false;

	// Test axis a02
	TYvec a02 = TYvec(0, -f2[Z], f2[Y]);
	p0 = glm::dot(v0, a02);
	p1 = glm::dot(v1, a02);
	p2 = glm::dot(v2, a02);
	r = box_extents[Y] * abs(f2[Z]) + box_extents[Z] * abs(f2[Y]);
	if (glm::max(-glm::max(glm::max(p0, p1), p2), glm::min(glm::min(p0, p1), p2)) > r)
		return false;

	// Test axis a10
	TYvec a10 = TYvec(f0[Z], 0, -f0[X]);
	p0 = glm::dot(v0, a10);
	p1 = glm::dot(v1, a10);
	p2 = glm::dot(v2, a10);;
	r = box_extents[X] * abs(f0[Z]) + box_extents[Z] * abs(f0[X]);
	if (glm::max(-glm::max(glm::max(p0, p1), p2), glm::min(glm::min(p0, p1), p2)) > r)
		return false;

	// Test axis a11
	TYvec a11 = TYvec(f1[Z], 0, -f1[X]);
	p0 = glm::dot(v0, a11);
	p1 = glm::dot(v1, a11);
	p2 = glm::dot(v2, a11);
	r = box_extents[X] * abs(f1[Z]) + box_extents[Z] * abs(f1[X]);
	if (glm::max(-glm::max(glm::max(p0, p1), p2), glm::min(glm::min(p0, p1), p2)) > r)
		return false;

	// Test axis a12
	TYvec a12 = TYvec(f2[Z], 0, -f2[X]);
	p0 = glm::dot(v0, a11);
	p1 = glm::dot(v1, a11);
	p2 = glm::dot(v2, a11);
	r = box_extents[X] * abs(f2[Z]) + box_extents[Z] * abs(f2[X]);
	if (glm::max(-glm::max(glm::max(p0, p1), p2), glm::min(glm::min(p0, p1), p2)) > r)
		return false;

	// Test axis a20
	TYvec a20 = TYvec(-f0[Y], f0[X], 0);
	p0 = glm::dot(v0, a20);
	p1 = glm::dot(v1, a20);
	p2 = glm::dot(v2, a20);
	r = box_extents[X] * abs(f0[Y]) + box_extents[Y] * abs(f0[X]);
	if (glm::max(-glm::max(glm::max(p0, p1), p2), glm::min(glm::min(p0, p1), p2)) > r)
		return false;

	// Test axis a21
	TYvec a21 = TYvec(-f1[Y], f1[X], 0);
	p0 = glm::dot(v0, a21);
	p1 = glm::dot(v1, a21);
	p2 = glm::dot(v2, a21);
	r = box_extents[X] * abs(f1[Y]) + box_extents[Y] * abs(f1[X]);
	if (glm::max(-glm::max(glm::max(p0, p1), p2), glm::min(glm::min(p0, p1), p2)) > r)
		return false;

	// Test axis a22;
	TYvec a22 = TYvec(-f2[Y], f2[X], 0);
	p0 = glm::dot(v0, a22);
	p1 = glm::dot(v1, a22);
	p2 = glm::dot(v2, a22);
	r = box_extents[X] * abs(f2[Y]) + box_extents[Y] * abs(f2[X]);
	if (glm::max(-glm::max(glm::max(p0, p1), p2), glm::min(glm::min(p0, p1), p2)) > r)
		return false;

//// region Test the three axes corresponding to the face normals of AABB b (category 1)

// Exit if...
// ... [-extents.X, extents.X] and [min(v0.X,v1.X,v2.X), max(v0.X,v1.X,v2.X)] do not overlap
	if (glm::max(glm::max(v0[X], v1[X]), v2[X]) < -box_extents[X] || glm::min(glm::min(v0[X], v1[X]), v2[X]) > box_extents[X])
		return false;

// ... [-extents.Y, extents.Y] and [min(v0.Y,v1.Y,v2.Y), max(v0.Y,v1.Y,v2.Y)] do not overlap
	if (glm::max(glm::max(v0[Y], v1[Y]), v2[Y]) < -box_extents[Y] || glm::min(glm::min(v0[Y], v1[Y]), v2[Y]) > box_extents[Y])
		return false;

// ... [-extents.Z, extents.Z] and [min(v0.Z,v1.Z,v2.Z), max(v0.Z,v1.Z,v2.Z)] do not overlap
	if (glm::max(glm::max(v0[Z], v1[Z]), v2[Z]) < -box_extents[Z] || glm::min(glm::min(v0[Z], v1[Z]), v2[Z]) > box_extents[Z])
		return false;

//// region Test separating axis corresponding to triangle face normal (category 2)

	TYvec plane_normal = glm::cross(f0, f1);
	TYfloat plane_distance = glm::dot(plane_normal, triangle.vertices[0].position);

		// Compute the projection interval radius of b onto L(t) = b.c + t * p.n
	r = box_extents[X] * abs(plane_normal[X]) + box_extents[Y] * abs(plane_normal[Y]) + box_extents[Z] * abs(plane_normal[Z]);

		// Intersection occurs when plane distance falls within[-r, +r] interval
	if (plane_distance > r)
		return false;

	return true;
}

Octree::Octree(Geometry* parentGeometry_) : parentGeometry(parentGeometry_)
{
	maxDepth = 0;
	root = TYnull;
	colorBias = 0.05f;
	colors = TYvector3(20);
	GenColors();
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

TYvoid Octree::Make()
{
	TYsizet count = GlobalTriangles.size();

	AABB aabb;
	aabb.Gen(GlobalTriangles);
	aabb.Planify();

	root = new Node(aabb, 0, GlobalTriangles);
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
		case 0://0:
			childAABB.Gen(parent->bound.min, dim);
			break;
		case 1:
			childAABB.Gen(parent->bound.min + TYvec(0, 0, dim.z), dim);
			break;
		case 2:
			childAABB.Gen(parent->bound.min + TYvec(0, dim.y, 0), dim);
			break;
		case 3://6:
			childAABB.Gen(parent->bound.min + TYvec(0, dim.y, dim.z), dim);
			break;
		case 4://1:
			childAABB.Gen(parent->bound.min + TYvec(dim.x, 0, 0), dim);
			break;
		case 5://3:
			childAABB.Gen(parent->bound.min + TYvec(dim.x, 0, dim.z), dim);
			break;
		case 6://5:
			childAABB.Gen(parent->bound.min + TYvec(dim.x, dim.y, 0), dim);
			break;
		case 7://7:
			childAABB.Gen(parent->bound.min + TYvec(dim.x, dim.y, dim.z), dim);
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
			return;
		}
		if (/*childTrisCount <= 200 ||*/ depth > 1)
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
			Node* child = new Node(childAABB, depth, childTris);
			child->childID = i;
			parent->children[i] = child;
			MakeRec(child, depth, childTris);
		}
	}
}

TYbool AABB::TriInAABB(Triangle& tri)
{
	TYvec half = (max - min) * 0.5f;

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

	return false;

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
			in++;
		}
	}
	return in >= 1;
	return true;
}


TYbool AABB::SegmentIntersectsGridAlignedBox3D(Segment segment)
{
	TYfloat tmin, tmax, tymin, tymax, tzmin, tzmax;
	TYvec bounds[] = { min, max };

	tmin = (bounds[segment.sign[0]].x - segment.origin.x) * segment.inverseDirection.x;
	tmax = (bounds[1 - segment.sign[0]].x - segment.origin.x) * segment.inverseDirection.x;
	tymin = (bounds[segment.sign[1]].y - segment.origin.y) * segment.inverseDirection.y;
	tymax = (bounds[1 - segment.sign[1]].y - segment.origin.y) * segment.inverseDirection.y;

	if ((tmin > tymax) || (tymin > tmax)) return false;

	if (tymin > tmin) tmin = tymin;

	if (tymax < tmax) tmax = tymax;

	tzmin = (bounds[segment.sign[2]].z - segment.origin.z) * segment.inverseDirection.z;
	tzmax = (bounds[1 - segment.sign[2]].z - segment.origin.z) * segment.inverseDirection.z;

	if ((tmin > tzmax) || (tzmin > tmax)) return false;

	if (tzmin > tmin) tmin = tzmin;

	if (tzmax < tmax)  tmax = tzmax;

	// this last check is different from the 'ray' case in below references: 
	// we need to check that the segment is on the span of the line
	// that intersects the box
	return !(tmax < 0.0f || tmin > 1.0f);
}


TYbool AABB::IntersectLineAABB(TYvec O, TYvec D, TYfloat t[], TYfloat epsilon)
{
	/*TYvec C = (min + max) * (TYfloat)0.5;
	TYvec e = (max - min) * (TYfloat)0.5;

	int parallel = 0;
	bool found = false;

	TYvec d = C - O;

	for (int i = 0; i < 3; ++i)
	{
		if (abs(D) < epsilon)
			parallel |= 1 << i;
		else
		{
			TYfloat es = (D > (TYfloat)0.0) ? e : -e;
			TYfloat invDi = (TYfloat)1.0 / D;
			if (!found)
			{
				t[0] = (d - es) * invDi;
				t[1] = (d + es) * invDi;
				found = true;
			}
			else
			{
				TYfloat s = (d - es) * invDi;
				if (s > t[0])
					t[0] = s;

				s = (d + es) * invDi;

				if (s < t[1])
					t[1] = s;
				if (t[0] > t[1])
					return false;
			}
		}
	}

	if (parallel)
		for (int i = 0; i < 3; ++i)
			if (parallel & (1 << i))
				if (abs(d - t[0] * D) > e || abs(d - t[1] * D) > e))
					return false;*/
	return true;
}




TYbool AABB::PointInAABB(TYvec point)
{
	TYvec half = (max - min) * 0.5f;

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

	if (node->depth != -1)
	{
		TYfloat width = 1.5f + (node->childID / 10.0f);

		if (node->color == TYnull)
		{
			color = colors[node->depth];
		}
		else
		{
			color = *node->color;
			//delete node->color;
			//node->color = TYnull;
		}

		color = colors[node->childID];

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

TYvoid Octree::Traverse(Node* node)
{
	if (node == TYnull) return;

	//TYlog << "Node: d: " << node->depth << " c: " << node->childID
	//	<< " t: " << node->Triangles.size() << " e: " << node->empty << "\n";

	printf("Node: d: %d | c: %d | t: %d | e: %d \n", node->depth, node->childID, (TYint)node->Triangles.size(), node->empty);

	if (node->depth != -1)
	{
		for (int i = 0; i < 8; i++)
		{
			Traverse(node->children[i]);
		}
	}
	else
	{

	}
}

TYvoid AABB::Gen(TYvector<Triangle>& triangles)
{
	TYvec mi(std::numeric_limits<TYfloat>::max());
	TYvec ma(-std::numeric_limits<TYfloat>::max());

	for (Triangle& tri : triangles)
	{
		for (Vertex& vert : tri.vertices.v)
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

TYbool Intersect(TYvec rayOrig, TYvec rayDir, AABB aabb, TYfloat& t)
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

Node* Node::Intersect(TYvec rayOrig, TYvec rayDir)
{
	if (empty)
	{
		//TYlog << "'" << childID << "'\n";
		if (Triangles.size() > 0)
		{
			return this;
		}
		return TYnull;
	}
	TYvec origin = rayOrig;

	//TYvector<Plane> planes = { bound.midPlanes[0], bound.midPlanes[1], bound.midPlanes[2] };

	TYfloat sx = glm::dot(origin, bound.midPlanes[2].Normal);
	TYfloat sy = glm::dot(origin, bound.midPlanes[1].Normal);
	TYfloat sz = glm::dot(origin, bound.midPlanes[0].Normal);

	TYvec side = TYvec(
		sx - bound.midPlanes[2].Distance >= -TYepsilon,
		sy - bound.midPlanes[1].Distance >= -TYepsilon,
		sz - bound.midPlanes[0].Distance >= -TYepsilon
	);

	TYfloat rdx = bound.midPlanes[2].RayDistance(origin, rayDir);
	TYfloat rdy = bound.midPlanes[1].RayDistance(origin, rayDir);
	TYfloat rdz = bound.midPlanes[0].RayDistance(origin, rayDir);

	TYfloat xDist = rdx;//(side.x == rayDir.x < 0.0f) ? rdx : TYinf; //
	TYfloat yDist = rdy;//(side.y == rayDir.y < 0.0f) ? rdy : TYinf; //
	TYfloat zDist = rdz;//(side.z == rayDir.z < 0.0f) ? rdz : TYinf; // 

	for (TYint i = 0; i < 3; i++)
	{
		TYint idx = (side.z ? 1 : 0) | (side.y ? 2 : 0) | (side.x ? 4 : 0);

		Node* ret = TYnull;
		if (children[idx])
			ret = children[idx]->Intersect(origin, rayDir);

		/*if (ret != TYnull)
			TYlog << ret->childID << "\n";*/

		if (ret != TYnull) return ret->Intersect(origin, rayDir);

		TYfloat minDist = glm::min(glm::min(xDist, yDist), zDist);
		if (minDist == TYinf) return TYnull;

		origin = rayOrig + rayDir * minDist;

		if (!bound.PointInAABB(origin)) return TYnull;

		if (minDist == xDist)
		{
			side.x = !side.x;
			xDist = TYinf;
		}
		else if (minDist == yDist)
		{
			side.y = !side.y;
			yDist = TYinf;
		}
		else if (minDist == zDist)
		{
			side.z = !side.z;
			zDist = TYinf;
		}
	}

	return TYnull;
}

Node* Octree::Intersect(TYvec rayOrig, TYvec rayDir)
{
	Node* node = TYnull;
	TYfloat t = 0.0f;
	TYfloat best_t = TYinf;

	if (::Intersect(rayOrig, rayDir, root->bound, t))
	{
		node = root;

		//node = root->Intersect(rayOrig, rayDir);

		return node;

		//TYfloat minDis = TYmaxf;
		//return root;

		for (TYint i = 0; i < 8; i++)
		{
			if (!root->children[i]) continue;
			if (::Intersect(rayOrig, rayDir, root->children[i]->bound, t))
			{
				if (t < best_t)
				{
					best_t = t;
					node = root->children[i];
					node->color = new TYvec(0.92f, 0.0f, 0.0f);
				}

				/*TYfloat dis = glm::distance(rayOrig, root->children[i]->bound.mid);
				if (dis < minDis)
				{
					minDis = dis;
					node = root->children[i];
					node->color = new TYvec(0.92f, 0.0f, 0.0f);
				}*/
			}
		}
		/*minDis = TYmaxf;
		Node* tnode = node;
		for (TYint i = 0; i < 8; i++)
		{
			if (!node->children[i]) continue;
			if (::Intersect(rayOrig, rayDir, node->children[i]->bound, t))
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
