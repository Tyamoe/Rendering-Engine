#include "Mesh.h"

#include "Geometry.h"
#include "Material.h"
#include "Animation.h"

#include "Scene.h"

#include "BoundingVolume.h"

#include "GLUtils.h"
#include "AssimpUtils.h"
#include "AnimationUtils.h"

static Assimp::Importer importer;

Mesh::Mesh(Geometry* geometry_, Material* mat)
{
	//geometry = geometry_;
	//geoList.push_back(geometry);

	if (!mat)
	{
		mat = new Material();
		TYvec4 randColor = { GetRand(0.0f, 1.0f), GetRand(0.0f, 1.0f), GetRand(0.0f, 1.0f), 1.0f };
		mat->color = randColor;
	}

	material = mat;

	if (GenHandles)
	{
		meshHandle = geometry_->GenHandle_GL();
	}

	SubMesh submesh;
	submesh.geometry = geometry_;
	submesh.OffsetIndex = 0;
	submesh.OffsetVertex = 0;
	submesh.NumIndices = (TYuint)geometry_->meshHandle.indexCount;

	subMeshes.push_back(submesh);

	isAnimated = false;
}

Mesh::Mesh(TYvector<Geometry*>& geoList_, Material* mat)
{
	//geoList = geoList_;
	//geometry = geoList_[0];

	if (!mat)
	{
		mat = new Material();
		TYvec4 randColor = { GetRand(0.0f, 1.0f), GetRand(0.0f, 1.0f), GetRand(0.0f, 1.0f), 1.0f };
		mat->color = randColor;
	}

	material = mat;

	SubMesh submesh;
	submesh.geometry = geoList_[0];
	submesh.OffsetIndex = 0;
	submesh.OffsetVertex = 0;
	submesh.NumIndices = (TYuint)geoList_[0]->meshHandle.indexCount;

	subMeshes.push_back(submesh);

	if (GenHandles)
	{
		meshHandle = geoList_[0]->GenHandle_GL();
	}

	isAnimated = false;
}

Mesh::Mesh(TYstring filename)
{

}

TYpair<Mesh*, Animation*> Mesh::CreateMesh(TYstring filename, TYbool& hasSkeleton, TYbool& hasAnimations, Material* material)
{
	// aiProcess_PreTransformVertices
	//ai_real ddd = importer.GetPropertyFloat("AI_CONFIG_GLOBAL_SCALE_FACTOR_KEY");
	//importer.SetPropertyFloat("AI_CONFIG_GLOBAL_SCALE_FACTOR_KEY", 1.01f);
	//ddd = importer.GetPropertyFloat("AI_CONFIG_GLOBAL_SCALE_FACTOR_KEY");
	const aiScene* scene = importer.ReadFile(filename.c_str(), aiProcessPreset_TargetRealtime_Fast | aiProcess_GenBoundingBoxes/* | aiProcess_GlobalScale*/);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		TYstring rre = importer.GetErrorString();
		std::cout << "ERROR::ASSIMP::" << rre << std::endl;
		std::cout << "ERROR::ASSIMP::" << std::endl;
	}
	aiScene* mScene = importer.GetOrphanedScene();

	hasAnimations = mScene->HasAnimations();
	hasSkeleton = mScene->mMeshes[0]->HasBones();

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
	Animation* anim = TYnull;
	if (hasAnimations)
	{
		anim = Animation::CreateAnimation(scene);
		anim->mScene = mScene;

		//anim->CreateSkeleton(mScene);

		TYmat globalInverseTransform = AssimpToGlm(mScene->mRootNode->mTransformation);
		anim->globalInvMatrix = glm::inverse(globalInverseTransform);
	}

	// Mesh
	Mesh* mesh = new Mesh();
	mesh->material = mat;

	TYvector<SubMesh> subMeshes;

	if (hasAnimations)
	{
		subMeshes.resize(mScene->mNumMeshes);

		TYvector<VertexAnim> gVertices;
		TYvector<TYuint> gIndices;

		TYuint NumVertices = 0;
		TYuint NumIndices = 0;

		for (TYuint i = 0; i < subMeshes.size(); i++)
		{
			subMeshes[i].NumIndices = mScene->mMeshes[i]->mNumFaces * 3;
			subMeshes[i].OffsetVertex = NumVertices;
			subMeshes[i].OffsetIndex = NumIndices;

			NumVertices += mScene->mMeshes[i]->mNumVertices;
			NumIndices += subMeshes[i].NumIndices;
		}

		gVertices.reserve(NumVertices);
		gIndices.reserve(NumIndices);

		for (TYuint i = 0; i < subMeshes.size(); i++)
		{
			const aiMesh* pMesh = mScene->mMeshes[i];
			subMeshes[i].geometry = new Model(i, pMesh, subMeshes[i].OffsetVertex, gVertices, gIndices, anim, Min, Max);
			subMeshes[i].geometry->surfaceColor = PixelColorF(randColor);
			subMeshes[i].geometry->bvh = new BVH();
			subMeshes[i].geometry->bvh->head = boundSphere;
		}

		MeshHandle mh;
		if (GenHandles)
		{
			TYuint VBO, IBO;

			glGenVertexArrays(1, &mh.VAO);
			glBindVertexArray(mh.VAO);

			glGenBuffers(1, &VBO);
			glGenBuffers(1, &IBO);

			glBindBuffer(GL_ARRAY_BUFFER, VBO);

			glBufferData(GL_ARRAY_BUFFER, gVertices.size() * sizeof(VertexAnim), &gVertices[0], GL_STATIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, gIndices.size() * sizeof(TYuint), &gIndices[0], GL_STATIC_DRAW);

			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glEnableVertexAttribArray(2);
			glEnableVertexAttribArray(3);
			glEnableVertexAttribArray(4);

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAnim), (GLvoid*)offsetof(VertexAnim, position));
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAnim), (GLvoid*)offsetof(VertexAnim, normal));
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexAnim), (GLvoid*)offsetof(VertexAnim, texCoord));
			glVertexAttribIPointer(3, 4, GL_INT, sizeof(VertexAnim), (GLvoid*)offsetof(VertexAnim, boneIds));
			glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(VertexAnim), (GLvoid*)offsetof(VertexAnim, boneWeights));

			glBindVertexArray(0);

			mh.indexCount = gIndices.size();
		}

		mesh->isAnimated = hasAnimations;
		mesh->subMeshes = subMeshes;
		mesh->meshHandle = mh;

		anim->currentPose.resize(anim->numBones);
	}
	else
	{
		subMeshes.resize(mScene->mNumMeshes);

		TYvector<Vertex> gVertices;
		TYvector<TYuint> gIndices;

		TYuint NumVertices = 0;
		TYuint NumIndices = 0;

		for (TYuint i = 0; i < subMeshes.size(); i++)
		{
			subMeshes[i].NumIndices = mScene->mMeshes[i]->mNumFaces * 3;
			subMeshes[i].OffsetVertex = NumVertices;
			subMeshes[i].OffsetIndex = NumIndices;

			NumVertices += mScene->mMeshes[i]->mNumVertices;
			NumIndices += subMeshes[i].NumIndices;
		}

		gVertices.reserve(NumVertices);
		gIndices.reserve(NumIndices);

		for (TYuint i = 0; i < subMeshes.size(); i++)
		{
			const aiMesh* pMesh = mScene->mMeshes[i];
			subMeshes[i].geometry = new Model(i, pMesh, gVertices, gIndices, Min, Max);
			subMeshes[i].geometry->surfaceColor = PixelColorF(randColor);
			subMeshes[i].geometry->bvh = new BVH();
			subMeshes[i].geometry->bvh->head = boundSphere;
		}

		MeshHandle mh;
		if (GenHandles)
		{
			TYuint VBO, IBO;

			glGenVertexArrays(1, &mh.VAO);
			glBindVertexArray(mh.VAO);

			glGenBuffers(1, &VBO);
			glGenBuffers(1, &IBO);

			glBindBuffer(GL_ARRAY_BUFFER, VBO);

			glBufferData(GL_ARRAY_BUFFER, gVertices.size() * sizeof(Vertex), &gVertices[0], GL_STATIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, gIndices.size() * sizeof(TYuint), &gIndices[0], GL_STATIC_DRAW);

			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glEnableVertexAttribArray(2);

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, position));
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, normal));
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, texCoord));

			glBindVertexArray(0);

			mh.indexCount = gIndices.size();
		}

		mesh->isAnimated = hasAnimations;
		mesh->subMeshes = subMeshes;
		mesh->meshHandle = mh;
	}

	/*TYvector<Geometry*> geometryList;
	Geometry* geo = TYnull;

	for (TYuint m = 0; m < mScene->mNumMeshes; m++)
	{
		geo = new Model(anim, mScene, mScene->mMeshes[m], hasAnimations, Min, Max);
		geo->surfaceColor = PixelColorF(randColor);
		geo->bvh = new BVH();
		geo->bvh->head = boundSphere;

		geometryList.push_back(geo);
	}

	Mesh* mesh = new Mesh(geometryList, mat); */

	return { mesh , anim };
}

Mesh::~Mesh()
{
	for (SubMesh& subMesh : subMeshes)
	{
		delete subMesh.geometry;
	}
	delete material;

	/*for (MeshHandle& meshHandle : meshHandles)
	{
		DestroyVAO(meshHandle.VAO);
	}*/
	DestroyVAO(meshHandle.VAO);
}

TYvoid Mesh::DestroyVAO(TYuint vao)
{
	GLint max_vtx_attrib = 0;
	GLuint buffer_object;

	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max_vtx_attrib);
	glBindVertexArray(vao);

	for (int i = 0; i < max_vtx_attrib; ++i)
	{
		glGetVertexAttribIuiv(i, GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING, &buffer_object);
		if (buffer_object > 0)
		{
			glDeleteBuffers(1, &buffer_object);
		}
	}

	glGetVertexArrayiv(vao, GL_ELEMENT_ARRAY_BUFFER_BINDING, reinterpret_cast<GLint*>(&buffer_object));

	if (buffer_object > 0)
	{
		glDeleteBuffers(1, &buffer_object);
	}

	glDeleteVertexArrays(1, &vao);
}

TYvoid Mesh::GenHandle_GL()
{
	//meshHandle = geometry->GenHandle_GL();

	/*for (Geometry* geo : geoList)
	{
		MeshHandle m = geo->GenHandle_GL();
		meshHandles.push_back(m);
	}
	meshHandle = meshHandles[0];*/
}

TYvoid Mesh::GenOctree()
{
	for (SubMesh& subMesh : subMeshes)
	{
		Geometry* geo = subMesh.geometry;
		if (geo->GetType() == geoModel)
		{
			static_cast<Model*>(geo)->GenOctree();
		}
	}
}

GeoType Mesh::geoType()
{
	return subMeshes[0].geometry->GetType();
}