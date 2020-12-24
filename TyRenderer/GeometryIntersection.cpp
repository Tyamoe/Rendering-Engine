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

	TYmat modelMat = transform->GetMatrix();

	//TYvector<Geometry*>& geoList = mesh->GetGeometryList();
	const TYvector<SubMesh>& subMeshList = mesh->GetSubMeshList();

	TYvec norm = TYvec(0.0f);
	TYfloat tnear = TYinf;
	Geometry* hit = TYnull;
	TYint hitIndex = -1;

	for (TYint i = 0; i < subMeshList.size(); i++)
	{
		Geometry* geometry = subMeshList[i].geometry;
		if (geometry->Intersect(rayOrig, rayDir, t0, t1, norm, modelMat))
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

TYbool Model::Intersect(TYvec rayOrig, TYvec rayDir, TYfloat& t0, TYfloat& t1, TYvec& normal, TYmat& modelMat)
{
	Node* node = TYnull;
	node = octree->Intersect(rayOrig, rayDir, modelMat);
	if (!node)
	{
		return false;
	}
	else
	{
		if (!node->color)
		{
			//node->color = new TYvec(TyVec3("fa68ff"));
		}
	}

	normal = node->hitInfo.normal;
	t0 = node->hitInfo.t0;
	t1 = node->hitInfo.t1;

	return true;
}

TYbool Triangle::Intersect(TYvec rayOrig, TYvec rayDir, TYfloat& t0, TYfloat& t1, TYvec& normal, TYmat& modelMat)
{
	TYvec v0 = modelMat * TYvec4(vertices[0].position, 1.0f);
	TYvec v1 = modelMat * TYvec4(vertices[1].position, 1.0f);
	TYvec v2 = modelMat * TYvec4(vertices[2].position, 1.0f);

	TYvec e1 = v1 - v0;
	TYvec e2 = v2 - v0;

	TYvec h = glm::cross(rayDir, e2);
	TYfloat det = glm::dot(e1, h);

	if (fabs(det) < TYepsilon)
		return false;

	TYfloat invDet = 1.0f / det;

	TYvec s = rayOrig - v0;

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
		return false;
	}

	normal = glm::cross(v1 - v0, v2 - v0);

	return true;
}

TYbool Sphere::Intersect(TYvec rayOrig, TYvec rayDir, TYfloat& t0, TYfloat& t1, TYvec& normal, TYmat& modelMat)
{
	TYvec c = center;// modelMat* TYvec4(center, 1.0f);
	TYvec l = c - rayOrig;

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
	normal = phit - c;

	return true;
}
