#pragma once

#ifndef TYAMOE3D

#include "Types.h"
#include "Utils.h"

#else

#include "Tyamoe3DHelper.h"
#include EngineInc(Types.h)
#include EngineInc(Utils.h)

#endif // TYAMOE3D

#include "RenderingUtils.h"
#include "Octree.h"
#include "MeshUtils.h"

class BVH;
class MeshHandle;
class Mesh;

struct aiScene;
struct aiMesh;

class Bone;
class Skeleton;
class Animation;
class Entity;

enum GeoType
{
	geoTriangle,
	geoSphere,
	geoModel
};

class Geometry
{
public:
	Geometry();
	Geometry(TYvec c, PixelColorF sc, TYfloat refl = 0, TYfloat transp = 0, PixelColorF ec = PixelColorF()) :
		center(c), surfaceColor(sc), emissionColor(ec), transparency(transp), reflection(refl)
	{ }
	virtual ~Geometry();

	virtual TYbool Intersect(TYvec rayOrig, TYvec rayDir, TYfloat& t0, TYfloat& t1, TYvec& normal) { return false; }

	virtual MeshHandle GenHandle_GL() {
		return MeshHandle();
	}
	virtual TYmat GetMatrix() { return TYmat(1.0f); }

	TYvoid SetType(GeoType pType) { type = pType; }
	GeoType GetType() { return type; }

	TYvoid AddVertex(Vertex pVertex);

	PixelColorF surfaceColor = PixelColorF();
	PixelColorF emissionColor = PixelColorF();

	TYfloat transparency = 0.0f;
	TYfloat reflection = 0.0f;

	TYvec center = TYvec(0.0f);

	MeshHandle meshHandle;

	BVH* bvh = TYnull;

	operator Mesh*();
	operator Mesh*() const;

protected:
	GeoType type;

public:
	static TYbool Intersect(Entity* entity, TYvec rayOrig, TYvec rayDir, TYfloat& t0, TYfloat& t1, TYvec& normal);
};

class Triangle : public Geometry
{
public:

	Triangle() : Geometry()
	{
		SetType(geoTriangle);
	}

	Triangle(Vertex v0, Vertex v1, Vertex v2);
	Triangle(TYvec c, Vertex v0, Vertex v1, Vertex v2, PixelColorF sc,
		TYfloat refl = 0, TYfloat transp = 0, PixelColorF ec = PixelColorF());
	~Triangle();

	TYbool Intersect(TYvec rayOrig, TYvec rayDir, TYfloat& t0, TYfloat& t1, TYvec& normal);

	MeshHandle GenHandle_GL();

	TYmat GetMatrix();

	TYfloat radius;
	TYfloat radiusSQR;

	union Vertice
	{
		Vertice() {}

		Vertex& operator[](TYint i)
		{
			return v[i];
		}

		struct
		{
			Vertex v0, v1, v2;
		};

		Vertex v[3];
	};

	Vertice vertices;

private:

};

class Sphere : public Geometry
{
public:
	Sphere() : Geometry()
	{
		SetType(geoSphere);
	}

	Sphere(TYvec c, TYfloat r, PixelColorF sc = PixelColorF(),
		TYfloat refl = 0, TYfloat transp = 0, PixelColorF ec = PixelColorF()) :
		radius(r), radiusSQR(r * r), Geometry(c, sc, refl, transp, ec)
	{
		SetType(geoSphere);
	}

	~Sphere();

	TYbool Intersect(TYvec rayOrig, TYvec rayDir, TYfloat& t0, TYfloat& t1, TYvec& normal);

	TYmat GetMatrix();

	MeshHandle GenHandle_GL();

	TYfloat radius;
	TYfloat radiusSQR;

private:

};

class Model : public Geometry
{
public:
	Model() : Geometry()
	{
		SetType(geoModel);

		octree = new Octree(this);
	}

	Model(Animation* anim, const aiScene* scene, aiMesh* mesh, TYbool hasAnimations, TYvec Min, TYvec Max);

	Model(TYstring filePath, PixelColorF sc,
		TYfloat refl = 0, TYfloat transp = 0, PixelColorF ec = PixelColorF(), TYvec ce = TYvec(0.0f));
	~Model();

	TYbool Intersect(TYvec rayOrig, TYvec rayDir, TYfloat& t0, TYfloat& t1, TYvec& normal);
	TYvoid AddTriangles(TYvector<Triangle>& pTriangles);

	TYmat GetMatrix();

	TYvoid GenOctree();
	MeshHandle GenHandle_GL();

	TYvector<Triangle> triangles;

	TYvectorUI Indices;
	TYvector<Vertex> Vertices;
	TYvector<VertexAnim> animVertices;

	TYfloat radius;
	TYfloat radiusSQR;

	TYbool hasAnimations = false;

	Octree* octree = TYnull;

private:

};
