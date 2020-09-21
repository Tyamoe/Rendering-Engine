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

typedef class BVH BVH;

enum GeoType
{
	geoTriangle,
	geoSphere,
	geoModel
};

class Vertex
{
public:
	Vertex(TYvec pVertex, TYvec pNormal = TYvec(), TYvec2 pTexCoords = TYvec2()) :
		position(pVertex), normal(pNormal), texCoord(pTexCoords)
	{ }

	TYvec position;
	TYvec normal;
	TYvec2 texCoord;
};

class MeshHandle
{
public:
	TYuint VAO = 0;
	TYuint Texture = 0;

	TYsizet indexCount = 0;
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

	virtual TYvoid GenHandle_GL() {}
	virtual TYmat GetMatrix() { return TYmat(1.0f); }

	TYvoid SetType(GeoType pType) { type = pType; }
	GeoType GetType() { return type; }

	TYvoid AddVertex(Vertex pVertex);

	template<GeoType t>
	void GetHandle()
	{
		if (t == geoModel)
		{
			TYlog << "HI";
		}
	}

	void ddd()
	{
		GetHandle<geoModel>();
	}

	PixelColorF surfaceColor = PixelColorF();
	PixelColorF emissionColor = PixelColorF();

	TYfloat transparency = 0.0f;
	TYfloat reflection = 0.0f;

	TYvec center = TYvec(0.0f);
	//TYvector<Vertex> vertices;

	MeshHandle meshHandle;

	BVH* bvh = TYnull;

protected:
	GeoType type;

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

	TYvoid GenHandle_GL();

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

	TYvoid GenHandle_GL();

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

	Model(TYstring filePath, PixelColorF sc,
		TYfloat refl = 0, TYfloat transp = 0, PixelColorF ec = PixelColorF(), TYvec ce = TYvec(0.0f));
	~Model();

	TYbool Intersect(TYvec rayOrig, TYvec rayDir, TYfloat& t0, TYfloat& t1, TYvec& normal);
	TYvoid AddTriangles(TYvector<Triangle>& pTriangles);

	TYmat GetMatrix();

	TYvoid GenOctree();
	TYvoid GenHandle_GL();

	TYvector<Triangle> triangles;

	TYvectorUI Indices;
	TYvector<Vertex> Vertices;

	TYfloat radius;
	TYfloat radiusSQR;

	Octree* octree = TYnull;

private:

};
