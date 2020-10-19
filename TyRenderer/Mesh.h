#pragma once

#ifndef TYAMOE3D

#include "Types.h"

#else

#include "Tyamoe3DHelper.h"
#include EngineInc(Types.h)

#endif // TYAMOE3D

#include "MeshUtils.h"

class Scene;
class Material;
class Animation;
class Geometry;
enum GeoType;
class Renderer;

class Mesh
{
public:
	Mesh(Geometry* geometry_ = TYnull, Material* mat = TYnull);
	Mesh(TYvector<Geometry*>& geoList_, Material* mat = TYnull);
	~Mesh();

	TYvoid GenHandle_GL();
	TYvoid GenOctree();

	Mesh(TYstring);

	GeoType geoType();

	TYsizet geoCount()
	{
		return geoList.size();
	}

	MeshHandle GetHandle()
	{
		return meshHandle;
	}

	MeshHandle GetHandle(TYint i)
	{
		return meshHandles[i];
	}

	Geometry* GetGeometry(TYint i = 0)
	{
		if (i == -1) return geoList[hitIndex];
		return geoList[i];
	}

	TYvoid SetHitIndex(TYint i) { hitIndex = i; }

	TYvector<Geometry*>& GetGeometryList()
	{
		return geoList;
	}

	TYbool IsAnimated()
	{
		return isAnimated;
	}

	friend class Scene;

private:
	TYvector<Geometry*> geoList;
	TYvector<MeshHandle> meshHandles;

	Geometry* geometry;
	Material* material;

	MeshHandle meshHandle;

	TYbool isAnimated = false;

	TYint hitIndex = 0;

	static TYpair<Mesh*, Animation*> CreateMesh(TYstring filename, TYbool&, TYbool&, Material* m = TYnull);

	friend class Renderer;

// Template
public:
	template<typename t>
	t Get(t p = 0)
	{
		return t();
	}

	template<>
	Material* Get(Material*)
	{
		return material;
	}

	template<>
	Geometry* Get(Geometry*)
	{
		return geometry;
	}
};

typedef Mesh* MeshPtr;
