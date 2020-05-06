#include "stdafx.h"

#include <fstream>

#include "Geometry.h"
#include "Globals.h"
#include "BVH.h"

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

Model::Model(TYstring filePath, PixelColorF sc, TYfloat refl, TYfloat transp, PixelColorF ec) :
	radius(0), radiusSQR(0), Geometry(TYvec(0.0f), sc, refl, transp, ec)
{
	SetType(geoModel);

	TYvectorF Vertices;
	TYvectorUI Indices;

	TYvectorF vertices;

	TYfloat minX = INFINITY, minY = INFINITY, minZ = INFINITY;
	TYfloat maxX = -INFINITY, maxY = -INFINITY, maxZ = -INFINITY;

	TYfloat MaxDistance = 0.0f;

	std::ifstream file(filePath);

	while (!file.eof())
	{
		TYstring input;
		std::getline(file, input);

		TYvectorS line;

		if (input[0] != 'v' && input[0] != 'f') continue;

		TYcharPtr cpy = _strdup(input.c_str());
		TYcharPtr tok = strtok(cpy, " \t");
		while (tok)
		{
			if (tok) line.push_back(tok);
			tok = strtok(NULL, " \t");
		}

		if (line[0] == "v")
		{
			TYvec vertex = { stof(line[1]), stof(line[2]), stof(line[3]) };

			TYfloat distance = glm::distance(vertex, TYvec(0, 0, 0));
			if (distance > MaxDistance)
			{
				MaxDistance = distance;
			}

			if (vertex.x > maxX) maxX = vertex.x;
			if (vertex.x < minX) minX = vertex.x;
			if (vertex.y > maxY) maxY = vertex.y;
			if (vertex.y < minY) minY = vertex.y;
			if (vertex.z > maxZ) maxZ = vertex.z;
			if (vertex.z < minZ) minZ = vertex.z;

			vertices.push_back(vertex.x);
			vertices.push_back(vertex.y);
			vertices.push_back(vertex.z);
		}
		else if (line[0] == "f")
		{
			TYint lineNum = 1;
			for (TYsizet i = 0; i < line[lineNum].length(); i++)
			{
				if (line[lineNum][i] == '/')
				{
					line[lineNum] = line[lineNum].substr(0, i);

					if (lineNum == 3) break;

					lineNum++;
					i = 0;
				}
			}

			Indices.push_back(stoi(line[1]) - 1);
			Indices.push_back(stoi(line[2]) - 1);
			Indices.push_back(stoi(line[3]) - 1);

			if (line.size() > 4 && line[4] != "")
			{
				for (TYsizet i = 4; i < line.size(); i++)
				{
					Indices.push_back(stoi(line[1]) - 1);
					Indices.push_back(stoi(line[i - 1]) - 1);
					Indices.push_back(stoi(line[i]) - 1);
				}
			}
		}
	}

	TYfloat w = std::abs(maxX - minX);
	TYfloat h = std::abs(maxY - minY);
	TYfloat d = std::abs(maxZ - minZ);

	TYfloat dx = maxX + minX;
	TYfloat dy = maxY + minY;
	TYfloat dz = maxZ + minZ;

	TYfloat sx = (dx / 2.0f);
	TYfloat sy = (dy / 2.0f);
	TYfloat sz = (dz / 2.0f);

	for (TYsizet i = 0; i < vertices.size(); i += 3)
	{
		TYfloat oldLow = 0.0f;
		TYfloat oldHigh = MaxDistance;
		TYfloat newLow = -5.0f;
		TYfloat newHigh = 5.0f;

		TYfloat x = vertices[i + 0];
		TYfloat y = vertices[i + 1];
		TYfloat z = vertices[i + 2];

		x -= sx;
		y -= sy;
		z -= sz;

		x = ((x - oldLow) / (oldHigh - oldLow)) * (newHigh - newLow + newLow);
		y = ((y - oldLow) / (oldHigh - oldLow)) * (newHigh - newLow + newLow);
		z = ((z - oldLow) / (oldHigh - oldLow)) * (newHigh - newLow + newLow);

		Vertices.push_back(x);
		Vertices.push_back(y);
		Vertices.push_back(z);
	}

	for (TYsizet i = 0; i < Indices.size(); i += 3)
	{
		TYuint i1 = Indices[i] * 3;
		TYuint i2 = Indices[i + 1] * 3;
		TYuint i3 = Indices[i + 2] * 3;

		TYvec v1 = TYvec(Vertices[i1], Vertices[i1 + 1], Vertices[i1 + 2] - 21.0f);
		TYvec v2 = TYvec(Vertices[i2], Vertices[i2 + 1], Vertices[i2 + 2] - 21.0f);
		TYvec v3 = TYvec(Vertices[i3], Vertices[i3 + 1], Vertices[i3 + 2] - 21.0f);

		triangles.push_back(Triangle(v1, v2, v3));
	}
}

TYvoid Model::AddTriangles(TYvector<Triangle>& pTriangles)
{
	triangles.insert(triangles.end(), pTriangles.begin(), pTriangles.end());
}

TYbool Model::Intersect(TYvec rayOrig, TYvec rayDir, TYfloat& t0, TYfloat& t1, TYvec& normal)
{
	if (bvh)
	{
		if (!bvh->head->Intersect(rayOrig, rayDir))
		{
			//TYlog << "Bounding Sphere\n";
			Global::CulledTries++;
			return false;
		}
	}

	//Global::CulledTries = 0;
	Global::TriCount = triangles.size();

	TYvec norm = TYvec(0.0f);
	TYfloat t00 = TYinf, t11 = TYinf;
	TYbool h = false;

	for (TYsizet i = 0; i < triangles.size(); i++)
	{
		TYvec p0 = triangles[i].vertices[0].position;
		TYvec p1 = triangles[i].vertices[1].position;
		TYvec p2 = triangles[i].vertices[2].position;

		/*TYvec prod = glm::normalize((glm::cross(p1 - p0, p2 - p0) * (p1 - TYvec(0.0f, 0.0f, -1.0f))));
		if (prod.z <= 0.0f)
		{
			//Global::CulledTries++;
			continue;
		}*/
		if (triangles[i].Intersect(rayOrig, rayDir, t0, t1, normal) && t0 < t00)
		{
			t00 = t0;
			t11 = t1;
			norm = normal;
			h = true;
			//return true;
		}
	}

	normal = norm;
	t0 = t00;
	t1 = t11;

	return h;
}

Model::~Model()
{

}

// TRIANGLE
Triangle::Triangle(Vertex v0, Vertex v1, Vertex v2) : Geometry()
{
	SetType(geoTriangle);

	AddVertex(v0);
	AddVertex(v1);
	AddVertex(v2);
}

Triangle::Triangle(TYvec c, Vertex v0, Vertex v1, Vertex v2, PixelColorF sc,
	TYfloat refl, TYfloat transp, PixelColorF ec) : Geometry((v0.position + v1.position + v2.position) / 3.0f, sc, refl, transp, ec)
{
	SetType(geoTriangle);

	AddVertex(v0);
	AddVertex(v1);
	AddVertex(v2);
}

TYbool Triangle::Intersect(TYvec rayOrig, TYvec rayDir, TYfloat& t0, TYfloat& t1, TYvec& normal)
{
	TYvec e1 = vertices[1].position - vertices[0].position;
	TYvec e2 = vertices[2].position - vertices[0].position;

	TYvec h = glm::cross(rayDir, e2);
	TYfloat det = glm::dot(e1, h);

	// Ray and triangle are parallel if det is close to 0
	if (fabs(det) < TYepsilon) 
		return false;

	TYfloat invDet = 1.0f / det;

	TYvec s = rayOrig - vertices[0].position;

	TYfloat u = glm::dot(s, h) * invDet;

	if (u < 0.0f || u > 1.0f)
		return false;

	TYvec qvec = glm::cross(s, e1);
	TYfloat v = glm::dot(rayDir, qvec) * invDet;

	if (v < 0 || u + v > 1) 
		return false;

	t0 = dot(e2, qvec) * invDet;

	if (t0 < TYepsilon)
	{
		Global::TriCount++;
		return false;
	}

	// Compute normal at the intersection point 
	normal = glm::cross(vertices[1].position - vertices[0].position, vertices[2].position - vertices[0].position);

	return true;
}

Triangle::~Triangle()
{

}

// SPHERE 
TYbool Sphere::Intersect(TYvec rayOrig, TYvec rayDir, TYfloat& t0, TYfloat& t1, TYvec& normal)
{
	/*if (bvh)
	{
		if (bvh->head->Intersect(rayOrig, rayDir))
		{
			//TYlog << "Bounding Sphere\n";
		}
	}*/

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

	if (t0 < 0)
	{
		t0 = t1;
	}

	// Compute normal at the intersection point 
	TYvec phit = rayOrig + rayDir * t0;
	normal = phit - center;

	return true;
}

Sphere::~Sphere()
{

}