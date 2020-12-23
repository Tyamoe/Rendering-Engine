#include "BVH.h"

#include "Types.h"

#include "Geometry.h"
#include "BoundingVolume.h"

BVH::~BVH()
{
	//delete head;
}

BVH* BVH::GenerateBVH(Sphere* sphere)
{
	BVH* bvh = new BVH();
	BoundingBox* box = new BoundingBox();

	box->min = sphere->center - sphere->radius;
	box->max = sphere->center + sphere->radius;
	box->center = sphere->center;
	box->extents = TYvec(sphere->radius * 2.0f);

	BoundingSphere* sp = new BoundingSphere();

	sp->radius = sphere->radius;
	sp->center = sphere->center;

	bvh->head = sp;

	return bvh;
}

BVH* BVH::GenerateBVH(Triangle* triangle)
{
	BVH* bvh = new BVH();
	BoundingBox* box = new BoundingBox();

	TYvec bmin(std::numeric_limits<TYfloat>::max());
	TYvec bmax(-std::numeric_limits<TYfloat>::max());

	TYvec v0 = triangle->vertices[0].position;
	TYvec v1 = triangle->vertices[1].position;
	TYvec v2 = triangle->vertices[2].position;

	for (const Vertex& vertex : triangle->vertices.v)
	{
		if (vertex.position.x > bmax.x) bmax.x = vertex.position.x;
		if (vertex.position.x < bmin.x) bmin.x = vertex.position.x;
		if (vertex.position.y > bmax.y) bmax.y = vertex.position.y;
		if (vertex.position.y < bmin.y) bmin.y = vertex.position.y;
		if (vertex.position.z > bmax.z) bmax.z = vertex.position.z;
		if (vertex.position.z < bmin.z) bmin.z = vertex.position.z;
	}

	box->min = bmin;
	box->max = bmax;
	box->center = (v0 + v1 + v2) / 3.0f;
	box->extents = (bmax - bmin);

	bvh->head = box;

	return bvh;
}

BVH* BVH::GenerateBVH(Model* model)
{
	BVH* bvh = new BVH();
	BoundingBox* box = new BoundingBox();

	TYvec bmin(std::numeric_limits<TYfloat>::max());
	TYvec bmax(-std::numeric_limits<TYfloat>::max());

	for (const Triangle& triangle : model->triangles)
	{
		for (const Vertex& vertex : triangle.vertices.v)
		{
			if (vertex.position.x > bmax.x) bmax.x = vertex.position.x;
			if (vertex.position.x < bmin.x) bmin.x = vertex.position.x;
			if (vertex.position.y > bmax.y) bmax.y = vertex.position.y;
			if (vertex.position.y < bmin.y) bmin.y = vertex.position.y;
			if (vertex.position.z > bmax.z) bmax.z = vertex.position.z;
			if (vertex.position.z < bmin.z) bmin.z = vertex.position.z;
		}
	}

	box->min = bmin;
	box->max = bmax;
	box->center = (bmax + bmin) / 2.0f;
	box->extents = (bmax - bmin);

	BoundingSphere* sp = new BoundingSphere();

	sp->radius = TYvec(bmax - bmin).x / 2.0f;
	sp->center = (bmax + bmin) / 2.0f;

	bvh->head = sp;

	return bvh;
}