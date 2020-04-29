#pragma once

#include "Types.h"
#include "RenderingUtils.h"
#include "Utils.h"

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
	Vertex(TYvec pVertex, TYvec pNormal = TYvec(0, 0, 1), TYvec2 pTexCoords = TYvec2()) :
		position(pVertex), normal(pNormal), texCoord(pTexCoords)
	{ }

	TYvec position;
	TYvec normal;
	TYvec2 texCoord;
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

	TYvoid SetType(GeoType pType) { type = pType; }
	GeoType GetType() { return type; }

	TYvoid AddVertex(Vertex pVertex);

	PixelColorF surfaceColor = PixelColorF();
	PixelColorF emissionColor = PixelColorF();

	TYfloat transparency = 0.0f;
	TYfloat reflection = 0.0f;

	TYvec center = TYvec(0.0f);
	TYvector<Vertex> vertices;

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

	TYfloat radius;
	TYfloat radiusSQR;

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
	}

	Model(TYstring filePath, PixelColorF sc,
		TYfloat refl = 0, TYfloat transp = 0, PixelColorF ec = PixelColorF());
	~Model();

	TYbool Intersect(TYvec rayOrig, TYvec rayDir, TYfloat& t0, TYfloat& t1, TYvec& normal);
	TYvoid AddTriangles(TYvector<Triangle>& pTriangles);

	TYvector<Triangle> triangles;

	TYfloat radius;
	TYfloat radiusSQR;

private:

};
