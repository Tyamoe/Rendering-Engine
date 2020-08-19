#pragma once

#include "Types.h"

#include "Mesh.h"

#include "Camera.h"
#include "Geometry.h"
#include "BVH.h"

class Scene
{
public:
	Scene();
	~Scene();

	TYvoid Init();

	CameraPtr camera;

	TYvector<Geometry*> geometry;
	TYvector<BVH*> bvh;

private:
	TYvoid AddGeometry(TYpair<Geometry*, BVH*> geo);
	TYvoid AddGeometry(Geometry* geo);
};

typedef Scene* ScenePtr;

template<typename T>
TYpair<Geometry*, BVH*> Make()
{
	T geo = new T();
	return { geo, TYnull };
}

template<typename T>
TYpair<Geometry*, BVH*> Make(TYvec c, TYfloat r)
{
	TYfloat color = GetRand<TYfloat>(0.64f, 0.9f);
	T* geo = new T(c, r, PixelColorF(color), 0.02f, 0.0f, PixelColorF());
	BVH* bvh = BVH::GenerateBVH(geo);

	geo->bvh = bvh;

	return { geo, bvh };
}