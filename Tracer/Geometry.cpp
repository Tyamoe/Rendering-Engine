#include "stdafx.h"

#include "Geometry.h"

// BASE
Geometry::Geometry()
{

}

TYvoid Geometry::AddVertex(Vertex pVertex)
{
	vertices.push_back(pVertex);
}

Geometry::~Geometry()
{

}

// TRIANGLE
Triangle::Triangle(TYvec c, Vertex v0, Vertex v1, Vertex v2, PixelColorF sc,
	TYfloat refl, TYfloat transp, PixelColorF ec) : Geometry(c, sc, refl, transp, ec)
{
	SetType(geoTriangle);

	AddVertex(v0);
	AddVertex(v1);
	AddVertex(v2);
}

TYbool Triangle::Intersect(TYvec rayOrig, TYvec rayDir, TYfloat& t0, TYfloat& t1)
{
	TYvec v0v1 = vertices[1].vertex - vertices[0].vertex;
	TYvec v0v2 = vertices[2].vertex - vertices[0].vertex;
	TYvec pvec = glm::cross(rayDir, v0v2);
	TYfloat det = glm::dot(v0v1, pvec);

	// ray and triangle are parallel if det is close to 0
	if (fabs(det) < TYepsilon) 
		return false;

	TYfloat invDet = 1 / det;

	TYvec tvec = rayOrig - vertices[0].vertex;
	t0 = glm::dot(tvec, pvec) * invDet;
	if (t0 < 0 || t0 > 1) 
		return false;

	TYvec qvec = glm::cross(tvec, v0v1);
	t1 = glm::dot(rayDir, qvec) * invDet;
	if (t1 < 0 || t0 + t1 > 1) 
		return false;

	t0 = dot(v0v2, qvec) * invDet;

	return true;
}

Triangle::~Triangle()
{

}

// SPHERE 
TYbool Sphere::Intersect(TYvec rayOrig, TYvec rayDir, TYfloat& t0, TYfloat& t1)
{
	TYvec l = center - rayOrig;

	TYfloat tca = glm::dot(l, rayDir);

	if (tca < 0) 
		return false;

	TYfloat d2 = glm::dot(l, l) - tca * tca;

	if (d2 > radiusSQR)
		return false;

	TYfloat thc = sqrt(radiusSQR - d2);

	t0 = tca - thc;
	t1 = tca + thc;

	return true;
}

Sphere::~Sphere()
{

}