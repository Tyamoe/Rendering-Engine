#pragma once

#include "Types.h"

#include "MeshUtils.h"

class Scene;
class Material;
class Animation;
class Geometry;
enum GeoType;
class Renderer;

struct SubMesh
{
	SubMesh() {}
	SubMesh(Geometry* geometry_, TYuint NumIndices_, TYuint OffsetVertex_, TYuint OffsetIndex_) : 
		geometry(geometry_), NumIndices(NumIndices_), OffsetVertex(OffsetVertex_), OffsetIndex(OffsetIndex_) {}

	Geometry* geometry;

	TYuint NumIndices = 0;

	TYuint OffsetVertex = 0;
	TYuint OffsetIndex = 0;

	TYuint MaterialIndex = 0;
};

class Mesh
{
public:
	friend class Scene;

	Mesh() {}
	Mesh(Geometry* geometry_, Material* mat);
	Mesh(TYvector<Geometry*>& geoList_, Material* mat = TYnull);
	~Mesh();

	static TYvoid DestroyVAO(TYuint vao);

	TYvoid GenHandle_GL();
	TYvoid GenOctree();

	Mesh(TYstring);

	GeoType geoType();

	TYsizet SubMeshCount()
	{
		return subMeshes.size();
	}

	MeshHandle& GetHandle()
	{
		return meshHandle;
	}

	Geometry* GetGeometry(TYint i = 0)
	{
		if (i == -1) return subMeshes[hitIndex].geometry;
		return  subMeshes[i].geometry;
	}

	const SubMesh& GetSubMesh(TYint i = 0)
	{
		if (i == -1) return subMeshes[hitIndex];
		return subMeshes[i];
	}
	
	const TYvector<SubMesh>& GetSubMeshList()
	{
		return subMeshes;
	}

	TYvoid SetHitIndex(TYint i) { hitIndex = i; }

	TYbool IsAnimated()
	{
		return isAnimated;
	}

private:
	friend class Renderer;

	TYvector<SubMesh> subMeshes;
	MeshHandle meshHandle;
	Material* material;

	TYbool isAnimated = false;

	TYint hitIndex = 0;

private:
	static TYpair<Mesh*, Animation*> CreateMesh(TYstring filename, TYbool&, TYbool&, Material* m = TYnull);

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

	static inline TYbool GenHandles = true;
};
