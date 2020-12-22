#include "Geometry.h"

#include "Octree.h"
#include "BVH.h"
#include "Entity.h"
#include "Mesh.h"
#include "Material.h"
#include "Transform.h"

#include "Globals.h"

TYbool Geometry::Intersect(Entity* entity, TYvec rayOrig, TYvec rayDir, TYfloat& t0, TYfloat& t1, TYvec& normal)
{
	Mesh* mesh = entity->Get<Mesh*>();
	Transform* transform = entity->Get<Transform*>();

	//TYvector<Geometry*>& geoList = mesh->GetGeometryList();
	const TYvector<SubMesh>& subMeshList = mesh->GetSubMeshList();

	TYvec norm = TYvec(0.0f);
	TYfloat tnear = TYinf;
	Geometry* hit = TYnull;
	TYint hitIndex = -1;

	for (TYint i = 0; i < subMeshList.size(); i++)
	{
		Geometry* geometry = subMeshList[i].geometry;
		if (geometry->Intersect(rayOrig, rayDir, t0, t1, norm))
		{
			if (t0 < 0)
			{
				t0 = t1;
			}

			if (t0 < tnear)
			{
				tnear = t0;
				hit = geometry;
				normal = norm;
				hitIndex = i;

				mesh->SetHitIndex(hitIndex);
			}
		}
	}

	return hit != TYnull;
}

TYbool Model::Intersect(TYvec rayOrig, TYvec rayDir, TYfloat& t0, TYfloat& t1, TYvec& normal)
{
	Node* node = TYnull;
	node = octree->Intersect(rayOrig, rayDir);
	if (!node)
	{
		Global::CulledTries++;
		return false;
	}
	else
	{
		if (!node->color)
		{
			node->color = new TYvec(TyVec3("fa68ff"));
		}

		if (node->empty)
		{
			//TYlog << "node->empty\n";
		}
	}

	//Global::TriCount = triangles.size();

	TYvec norm = TYvec(0.0f);
	TYfloat t00 = TYinf, t11 = TYinf;
	TYbool h = false;

	if (node && node->Triangles.size() <= triangles.size())
	{
		Global::TriCount++;
	}

	//for (TYsizet i = 0; i < triangles.size(); i++)
	for (TYsizet i = 0; i < node->Triangles.size(); i++)
	{
		/*TYvec p0 = triangles[i].vertices[0].position;
		TYvec p1 = triangles[i].vertices[1].position;
		TYvec p2 = triangles[i].vertices[2].position;

		TYvec prod = glm::normalize((glm::cross(p1 - p0, p2 - p0) * (p1 - TYvec(0.0f, 0.0f, -1.0f))));
		if (prod.z <= 0.0f)
		{
			//Global::CulledTries++;
			continue;
		}*/
		if (node->Triangles[i].Intersect(rayOrig, rayDir, t0, t1, normal) && t0 < TYmaxf && t0 < t00)
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

	/*if (!h)
	{
		t0 = node->T;
		h = true;
	}*/

	return h;
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

	normal = glm::cross(vertices[1].position - vertices[0].position, vertices[2].position - vertices[0].position);

	return true;
}

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
