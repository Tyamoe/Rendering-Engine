#include "Geometry.h"

#include <fstream>

#include "BVH.h"
#include "Octree.h"
#include "Debugger.h"

#include "Mesh.h"
#include "Animation.h"

#include "Globals.h"
#include "AssimpUtils.h"
#include "AnimationUtils.h"
#include "GLUtils.h"

static MeshHandle sphereHandle;

// BASE
Geometry::Geometry()
{

}

TYvoid Geometry::AddVertex(Vertex pVertex)
{
	//vertices.push_back(pVertex);
}

Geometry::~Geometry()
{
	delete bvh;
}

Geometry::operator Mesh*()
{
	return new Mesh(this, TYnull);
}

Geometry::operator Mesh*() const
{
	return new Mesh((Geometry*)this, TYnull);
}

// TRIANGLE
Triangle::Triangle(Vertex v0_, Vertex v1_, Vertex v2_) : Geometry()
{
	SetType(geoTriangle);

	AddVertex(v0_);
	AddVertex(v1_);
	AddVertex(v2_);

	vertices.v0 = v0_;
	vertices.v1 = v1_;
	vertices.v2 = v2_;

	TYvec vector1, vector2, crossProduct;

	vector1 = v0_.position - v1_.position;
	vector2 = v0_.position - v2_.position;
	crossProduct = glm::cross(vector1, vector2);
	crossProduct = glm::normalize(crossProduct);

	vertices.v0.normal = crossProduct;
	vertices.v1.normal = crossProduct;
	vertices.v2.normal = crossProduct;
}

Triangle::Triangle(TYvec c, Vertex v0, Vertex v1, Vertex v2, PixelColorF sc,
	TYfloat refl, TYfloat transp, PixelColorF ec) : Geometry((v0.position + v1.position + v2.position) / 3.0f, sc, refl, transp, ec)
{
	SetType(geoTriangle);

	AddVertex(v0);
	AddVertex(v1);
	AddVertex(v2);

	vertices.v[0] = v0;
	vertices.v[1] = v1;
	vertices.v[2] = v2;
}

TYmat Triangle::GetMatrix()
{
	TYmat model = TYmat(1.0f);
	model = glm::translate(model, { 0.0f, 0.0f, 0.0f });
	model = glm::scale(model, { 1.0f, 1.0f, 1.0f });

	return model;
}

MeshHandle Triangle::GenHandle_GL()
{
	TYvectorUI Indices = { 0, 1, 2 };

	MeshHandle mh;

	TYuint VBO, IBO;

	glGenVertexArrays(1, &mh.VAO);
	glBindVertexArray(mh.VAO);

	glGenBuffers(1, &VBO);
	glGenBuffers(1, &IBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertice), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.size() * sizeof(TYuint), &Indices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(TYfloat), (TYvoid*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(TYfloat), (TYvoid*)(3 * sizeof(TYfloat)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(TYfloat), (TYvoid*)(6 * sizeof(TYfloat)));

	glBindVertexArray(0);

	mh.indexCount = Indices.size();

	meshHandle = mh;

	return meshHandle;
}

Triangle::~Triangle()
{

}

// SPHERE 
TYmat Sphere::GetMatrix()
{
	TYmat model = TYmat(1.0f);
	model = glm::translate(model, center);
	model = glm::scale(model, { radius, radius, radius });

	return model;
}

MeshHandle Sphere::GenHandle_GL()
{
	if (sphereHandle.indexCount == 0)
	{
		TYvectorUI Indices;
		TYvector<Vertex> Vertices;

		TYvectorF SphereVertices;
		TYfloat radius = 1.0f;
		TYint stackCount = 20;
		TYint sectorCount = 25;

		TYfloat x, y, z, xy, lengthInv = 1.0f / radius;

		TYfloat sectorStep = 2.0f * glm::pi<TYfloat>() / sectorCount;
		TYfloat stackStep = glm::pi<TYfloat>() / stackCount;
		TYfloat sectorAngle, stackAngle;

		for (TYint i = 0; i <= stackCount; i++)
		{
			stackAngle = glm::pi<TYfloat>() / 2 - i * stackStep;
			xy = radius * glm::cos(stackAngle);
			z = radius * glm::sin(stackAngle);

			for (TYint j = 0; j <= sectorCount; j++)
			{
				sectorAngle = j * sectorStep;

				x = xy * glm::cos(sectorAngle);
				y = xy * glm::sin(sectorAngle);

				TYvec norm = TYvec(x * lengthInv, y * lengthInv, z * lengthInv);
				norm = glm::normalize(norm);

				TYvec2 uv;
				uv.x = atan2(norm.x, norm.z) / (2.0f * TYpi) + 0.5f;
				uv.y = norm.y * 0.5f + 0.5f;

				Vertices.push_back(Vertex(TYvec(x, y, z), norm, uv));
			}
		}

		TYint k1, k2;
		for (TYint i = 0; i < stackCount; i++)
		{
			k1 = i * (sectorCount + 1);
			k2 = k1 + sectorCount + 1;

			for (TYint j = 0; j < sectorCount; j++, k1++, k2++)
			{
				if (i != 0)
				{
					Indices.push_back(k1);
					Indices.push_back(k2);
					Indices.push_back(k1 + 1);
				}

				if (i != (stackCount - 1))
				{
					Indices.push_back(k1 + 1);
					Indices.push_back(k2);
					Indices.push_back(k2 + 1);
				}
			}
		}

		MeshHandle mh;

		TYuint VBO, IBO;

		glGenVertexArrays(1, &mh.VAO);
		glBindVertexArray(mh.VAO);

		glGenBuffers(1, &VBO);
		glGenBuffers(1, &IBO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(Vertex), &Vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.size() * sizeof(TYuint), &Indices[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(TYfloat), (TYvoid*)0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(TYfloat), (TYvoid*)(3 * sizeof(TYfloat)));
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(TYfloat), (TYvoid*)(6 * sizeof(TYfloat)));

		glBindVertexArray(0);

		mh.indexCount = Indices.size();

		sphereHandle = mh;
	}

	meshHandle = sphereHandle;

	return meshHandle;
}

Sphere::~Sphere()
{

}