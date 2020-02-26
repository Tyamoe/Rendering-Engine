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
	TYvec e1 = vertices[1].vertex - vertices[0].vertex;
	TYvec e2 = vertices[2].vertex - vertices[0].vertex;

	TYvec h = glm::cross(rayDir, e2);
	TYfloat det = glm::dot(e1, h);

	// ray and triangle are parallel if det is close to 0
	if (fabs(det) < TYepsilon) 
		return false;

	TYfloat invDet = 1.0f / det;

	TYvec s = rayOrig - vertices[0].vertex;

	TYfloat u = glm::dot(s, h) * invDet;

	if (u < 0.0f || u > 1.0f)
		return false;

	TYvec qvec = glm::cross(s, e1);
	TYfloat v = glm::dot(rayDir, qvec) * invDet;

	if (v < 0 || u + v > 1) 
		return false;

	t0 = dot(e2, qvec) * invDet;

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