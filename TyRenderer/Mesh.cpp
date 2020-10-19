#include "Tyamoe3DHelper.h"

#ifndef TYAMOE3D

#include "stdafx.h"

#else

#include EngineInc(stdafx.h)

#endif // TYAMOE3D

#include "Scene.h"
#include "Mesh.h"
#include "Geometry.h"
#include "Material.h"
#include "Animation.h"

#include "AssimpUtils.h"
#include "AnimationUtils.h"

static Assimp::Importer importer;

Mesh::Mesh(Geometry* geometry_, Material* mat)
{
	geometry = geometry_;
	geoList.push_back(geometry);
	material = mat;
}

Mesh::Mesh(TYvector<Geometry*>& geoList_, Material* mat)
{
	geoList = geoList_;
	geometry = geoList_[0];
	material = mat;
}

Mesh::Mesh(TYstring filename)
{

}

TYpair<Mesh*, Animation*> Mesh::CreateMesh(TYstring filename, TYbool& hasSkeleton, TYbool& hasAnimations, Material* material)
{
	// aiProcess_PreTransformVertices
	ai_real ddd = importer.GetPropertyFloat("AI_CONFIG_GLOBAL_SCALE_FACTOR_KEY");
	importer.SetPropertyFloat("AI_CONFIG_GLOBAL_SCALE_FACTOR_KEY", 0.01f);
	ddd = importer.GetPropertyFloat("AI_CONFIG_GLOBAL_SCALE_FACTOR_KEY");
	const aiScene* scene = importer.ReadFile(filename.c_str(), aiProcessPreset_TargetRealtime_Fast | aiProcess_GenBoundingBoxes | aiProcess_GlobalScale);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		TYstring rre = importer.GetErrorString();
		std::cout << "ERROR::ASSIMP::" << rre << std::endl;
		std::cout << "ERROR::ASSIMP::" << std::endl;
	}
	aiScene* mScene = importer.GetOrphanedScene();

	hasAnimations = mScene->HasAnimations();
	hasSkeleton = mScene->mMeshes[0]->HasBones();

	TYbool fbx = filename.find("fbx") != TYstring::npos;

	TYbool hasMaterial = mScene->HasMaterials();
	Material* mat = new Material();

	TYvec4 randColor = { GetRand(0.0f, 1.0f), GetRand(0.0f, 1.0f), GetRand(0.0f, 1.0f), 1.0f };

	if (hasMaterial && !material)
	{
		if (mScene->mMeshes[0]->HasVertexColors(0))
		{
			mat->color = AssimpToGlm(*mScene->mMeshes[0]->mColors[0]);
		}
		else
		{
			mat->color = randColor;
		}
	}
	else if(!material)
	{
		mat->color = randColor;
	}
	else
	{
		delete mat;
		mat = material;
	}

	// Bounding Box
	TYvec Min = TYvec(TYinf), Max = TYvec(-TYinf);
	for (TYuint m = 0; m < mScene->mNumMeshes; m++)
	{
		TYvec boundsMin = AssimpToGlm(mScene->mMeshes[m]->mAABB.mMin);
		TYvec boundsMax = AssimpToGlm(mScene->mMeshes[m]->mAABB.mMax);

		Min.x = boundsMin.x < Min.x ? boundsMin.x : Min.x;
		Min.y = boundsMin.y < Min.y ? boundsMin.y : Min.y;
		Min.z = boundsMin.z < Min.z ? boundsMin.z : Min.z;

		Max.x = boundsMax.x > Max.x ? boundsMax.x : Max.x;
		Max.y = boundsMax.y > Max.y ? boundsMax.y : Max.y;
		Max.z = boundsMax.z > Max.z ? boundsMax.z : Max.z;
	}
	BoundingSphere* boundSphere = new BoundingSphere(Min, Max);

	// Animation
	AnimationPtr anim = TYnull;
	if (hasAnimations)
	{
		anim = Animation::CreateAnimation(scene, fbx);
		anim->mScene = mScene;

		anim->CreateSkeleton(mScene);

		TYmat globalInverseTransform = AssimpToGlm(mScene->mRootNode->mTransformation);
		anim->globalInvMatrix = glm::inverse(globalInverseTransform);
	}

	// Mesh
	TYvector<Geometry*> geometryList;
	Geometry* geo = TYnull;

	for (TYuint m = 0; m < mScene->mNumMeshes; m++)
	{
		geo = new Model(anim, mScene, mScene->mMeshes[m], hasAnimations, Min, Max);
		geo->surfaceColor = PixelColorF(randColor);
		geo->bvh = new BVH();
		geo->bvh->head = boundSphere;

		geometryList.push_back(geo);
	}

	MeshPtr mesh = new Mesh(geometryList, mat);
	mesh->isAnimated = hasAnimations;

	return { mesh , anim };
}

Mesh::~Mesh()
{

}

TYvoid Mesh::GenHandle_GL()
{
	//meshHandle = geometry->GenHandle_GL();

	for (Geometry* geo : geoList)
	{
		MeshHandle m = geo->GenHandle_GL();
		meshHandles.push_back(m);
	}
	meshHandle = meshHandles[0];
}

TYvoid Mesh::GenOctree()
{
	for (Geometry* geo : geoList)
	{
		if (geo->GetType() == geoModel)
		{
			static_cast<Model*>(geo)->GenOctree();
		}
	}
}

GeoType Mesh::geoType()
{
	return geometry->GetType();
}