#pragma once

#include "Types.h"
#include "RendererRayTraceCPU.h"

class Vertex
{
public:
	Vertex(TYvec pVertex, TYvec pNormal = TYvec(), TYvec2 pTexCoords = TYvec2()) :
		vertex(pVertex), normal(pNormal), texCoord(pTexCoords) 
	{ }

	TYvec vertex;
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

	virtual TYbool Intersect(TYvec rayOrig, TYvec rayDir, TYfloat& t0, TYfloat& t1) { return false; }

	TYvoid AddVertex(Vertex pVertex);

	PixelColorF surfaceColor;
	PixelColorF emissionColor;

	TYfloat transparency;
	TYfloat reflection;

	TYvec center;

protected:
	TYvector<Vertex> vertices;

};

class Triangle : public Geometry
{
public:
	Triangle(TYvec c, Vertex v0, Vertex v1, Vertex v2, PixelColorF sc,
		TYfloat refl = 0, TYfloat transp = 0, PixelColorF ec = PixelColorF());
	~Triangle();

	TYbool Intersect(TYvec rayOrig, TYvec rayDir, TYfloat& t0, TYfloat& t1);

	TYfloat radius;
	TYfloat radiusSQR;

private:
};

class Sphere : public Geometry
{
public:
	~Sphere();

	Sphere(TYvec c, TYfloat r, PixelColorF sc, 
		TYfloat refl = 0, TYfloat transp = 0, PixelColorF ec = PixelColorF()) : 
		radius(r), radiusSQR(r * r), Geometry(c, sc, refl, transp, ec)
	{ 
		
	}

	TYbool Intersect(TYvec rayOrig, TYvec rayDir, TYfloat& t0, TYfloat& t1);

	TYfloat radius;
	TYfloat radiusSQR;
};

class Model : public Triangle
{
	Model();
	~Model();

	TYbool Intersect(TYvec rayOrig, TYvec rayDir, TYfloat& t0, TYfloat& t1) { return false; }
};