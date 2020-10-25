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

}

Geometry::operator Mesh*()
{
	return new Mesh(this);
}

Geometry::operator Mesh*() const
{
	return new Mesh((Geometry*)this);
}

// Model
Model::Model() : Geometry()
{
	SetType(geoModel);

	octree = new Octree(this);
}

Model::Model(TYstring filePath, PixelColorF sc, TYfloat refl, TYfloat transp, PixelColorF ec, TYvec ce) :
	radius(0), radiusSQR(0), Geometry(ce, sc, refl, transp, ec)
{
	SetType(geoModel);

	octree = new Octree(this);

	TYbool incNormals = false;
	TYbool incTexCoords = false;

	TYvector3 tVertices;
	TYvector3 Normals;

	TYvector<TYvec2> TexCoords;
	//TYvectorUI Indices;

	TYvectorF vertices;

	TYvector3 NormalSum;

	TYfloat minX = INFINITY, minY = INFINITY, minZ = INFINITY;
	TYfloat maxX = -INFINITY, maxY = -INFINITY, maxZ = -INFINITY;

	TYfloat MaxDistance = 0.0f;

	TYvectorUI Indices11;

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

		if (line[0] == "vn")
		{
			TYvec normal = { stof(line[1]), stof(line[2]), stof(line[3]) };
			Normals.push_back(normal);

			incNormals = true;
		}
		else if (line[0] == "vt")
		{
			TYvec2 texCoord = { stof(line[1]), stof(line[2]) };
			TexCoords.push_back(texCoord);

			incTexCoords = true;
		}
		else if (line[0] == "v")
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

			if(!incNormals)
				NormalSum.push_back(TYvec(0.0f, 0.0f, 0.0f));
		}
		else if (line[0] == "f")
		{
			TYvector<TYvectorI> faces(line.size() - 1);

			TYint lineNum = 1;
			for (TYsizet i = 1; i < line.size(); i++)
			{
				TYcharPtr cpy1 = _strdup(line[i].c_str());
				TYcharPtr tok1= strtok(cpy1, "/\\");
				while (tok1)
				{
					if (tok1) faces[i-1].push_back(stoi((TYstring)tok1));
					tok1 = strtok(NULL, "/\\");
				}
			}

			/*lineNum = 1;
			for (TYsizet i = 1; i < line.size(); i++)
			{
				if (line[lineNum][i] == '/')
				{
					line[lineNum] = line[lineNum].substr(0, i);

					if (lineNum == 3) break;

					lineNum++;
					i = 0;
				}
			}*/
			
			Indices.push_back(faces[0][0] - 1);
			Indices.push_back(faces[1][0] - 1);
			Indices.push_back(faces[2][0] - 1);

			if (faces.size() > 3)
			{
				for (TYsizet i = 3; i < faces.size(); i++)
				{
					Indices.push_back(faces[0    ][0] - 1);
					Indices.push_back(faces[i - 1][0] - 1);
					Indices.push_back(faces[i    ][0] - 1);
				}
			}

			/*Indices.push_back(stoi(line[1]) - 1);
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
			}*/
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

	// - - - - - - - 
	if (!incNormals)
	{
		TYvec vertex1, vertex2, vertex3, vector1, vector2, crossProduct;
		for (TYsizet i = 0; i < Indices.size(); i += 3)
		{
			TYuint index1 = Indices[i] * 3;
			TYuint index2 = Indices[i + 1] * 3;
			TYuint index3 = Indices[i + 2] * 3;

			vertex1 = TYvec(vertices[index1], vertices[index1 + 1], vertices[index1 + 2]);
			vertex2 = TYvec(vertices[index2], vertices[index2 + 1], vertices[index2 + 2]);
			vertex3 = TYvec(vertices[index3], vertices[index3 + 1], vertices[index3 + 2]);

			vector1 = vertex1 - vertex2;
			vector2 = vertex1 - vertex3;
			crossProduct = glm::cross(vector1, vector2);
			normalize(crossProduct);

			index1 = index1 / 3;
			index2 = index2 / 3;
			index3 = index3 / 3;

			NormalSum[index1] += crossProduct;
			NormalSum[index2] += crossProduct;
			NormalSum[index3] += crossProduct;
		}
	}

	TYint ni = 0;
	for (TYsizet i = 0; i < vertices.size(); i += 3)
	{
		TYfloat oldLow = 0.0f;
		TYfloat oldHigh = MaxDistance;
		TYfloat newLow = -2.0f;
		TYfloat newHigh = 2.0f;

		TYfloat x = vertices[i + 0];
		TYfloat y = vertices[i + 1];
		TYfloat z = vertices[i + 2];

		x -= sx;
		y -= sy;
		z -= sz;

		x = ((x - oldLow) / (oldHigh - oldLow)) * (newHigh - newLow + newLow);
		y = ((y - oldLow) / (oldHigh - oldLow)) * (newHigh - newLow + newLow);
		z = ((z - oldLow) / (oldHigh - oldLow)) * (newHigh - newLow + newLow);

		TYvec nn = glm::normalize(NormalSum[i / 3]);

		tVertices.push_back(TYvec(x, y, z));

		if (!incNormals)
		{
			Normals.push_back(nn);
			Vertices.push_back(Vertex(TYvec(x, y, z), nn));
		}
		else 
		{
			if (!incTexCoords)
				Vertices.push_back(Vertex(TYvec(x, y, z), Normals[ni]));
			else
				Vertices.push_back(Vertex(TYvec(x, y, z), Normals[ni], TexCoords[ni]));
			ni++;
		}
	}

	for (TYsizet i = 0; i < Indices.size(); i += 3)
	{
		TYuint i1 = Indices[i];

		TYuint i2 = Indices[i + 1];

		TYuint i3 = Indices[i + 2];

		//TYvec v1 = TYvec(Vertices[i1], Vertices[i1 + 1], Vertices[i1 + 2] - 21.0f);
		//TYvec v2 = TYvec(Vertices[i2], Vertices[i2 + 1], Vertices[i2 + 2] - 21.0f);
		//TYvec v3 = TYvec(Vertices[i3], Vertices[i3 + 1], Vertices[i3 + 2] - 21.0f);

		Vertex v1 = Vertex(tVertices[i1], Normals[i1], incTexCoords ? TexCoords[i1] : TYvec2());
		Vertex v2 = Vertex(tVertices[i2], Normals[i2], incTexCoords ? TexCoords[i2] : TYvec2());
		Vertex v3 = Vertex(tVertices[i3], Normals[i3], incTexCoords ? TexCoords[i3] : TYvec2());

		triangles.push_back(Triangle(v1, v2, v3));
		//octree->GlobalTriangles.push_back(Triangle(v1, v2, v3));
	}
	//octree->Make();
}

Model::Model(Animation* anim, const aiScene* scene, aiMesh* mesh, TYbool hasAnimations_, TYvec Min, TYvec Max)
{
	SetType(geoModel);
	octree = new Octree(this);

	hasAnimations = hasAnimations_;

	Skeleton* skeleton = TYnull;
	if (anim)
	{
		skeleton = anim->skeleton;
	}

	TYvec Centering = (Max + Min) / 2.0f;

	if (hasAnimations)
	{
		for (TYuint i = 0; i < mesh->mNumVertices; i++)
		{
			VertexAnim vertex;

			//process position 
			TYvec vector = AssimpToGlm(mesh->mVertices[i]) - Centering;
			vertex.position = vector;

			vector = TYvec();
			//process normal
			if (mesh->HasNormals())
			{
				vector.x = mesh->mNormals[i].x;
				vector.y = mesh->mNormals[i].y;
				vector.z = mesh->mNormals[i].z;
			}
			vertex.normal = vector;

			//process uv
			TYvec2 vec = TYvec();
			if (mesh->HasTextureCoords(0))
			{
				vec.x = mesh->mTextureCoords[0][i].x;
				vec.y = mesh->mTextureCoords[0][i].y;
			}
			vertex.texCoord = vec;

			vertex.boneIds = glm::ivec4(0);
			vertex.boneWeights = TYvec4(0.0f);

			animVertices.push_back(vertex);
		}
	}
	else 
	{
		for (TYuint i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;

			//process position 
			TYvec vector = AssimpToGlm(mesh->mVertices[i]) - Centering;
			vertex.position = vector;

			vector = TYvec();
			//process normal
			if (mesh->HasNormals())
			{
				vector.x = mesh->mNormals[i].x;
				vector.y = mesh->mNormals[i].y;
				vector.z = mesh->mNormals[i].z;
			}
			vertex.normal = vector;

			//process uv
			TYvec2 vec = TYvec();
			if (mesh->HasTextureCoords(0))
			{
				vec.x = mesh->mTextureCoords[0][i].x;
				vec.y = mesh->mTextureCoords[0][i].y;
			}
			vertex.texCoord = vec;

			Vertices.push_back(vertex);
		}
	}

	if (hasAnimations)
	{
		TYvector<TYuint> boneCounts;

		boneCounts.resize(animVertices.size(), 0);

		TYuint nBoneCount = mesh->mNumBones;

		for (TYuint i = 0; i < nBoneCount; i++)
		{
			aiBone* bone = mesh->mBones[i];
			TYint hashStr = Hash(TYstring(bone->mName.C_Str()));

			for (TYuint j = 0; j < bone->mNumWeights; j++)
			{
				TYuint id = bone->mWeights[j].mVertexId;
				TYfloat weight = bone->mWeights[j].mWeight;

				boneCounts[id]++;

				switch (boneCounts[id])
				{
				case 1:
					animVertices[id].boneIds.x = skeleton->skeleton[hashStr]->id;;
					animVertices[id].boneWeights.x = weight;
					break;
				case 2:
					animVertices[id].boneIds.y = skeleton->skeleton[hashStr]->id;;
					animVertices[id].boneWeights.y = weight;
					break;
				case 3:
					animVertices[id].boneIds.z = skeleton->skeleton[hashStr]->id;;
					animVertices[id].boneWeights.z = weight;
					break;
				case 4:
					animVertices[id].boneIds.w = skeleton->skeleton[hashStr]->id;;
					animVertices[id].boneWeights.w = weight;
					break;
				default:
					//std::cout << "err: unable to allocate bone to vertex" << std::endl;
					break;
				}
			}
		}
		
		for (TYsizet i = 0; i < animVertices.size(); i++)
		{
			TYvec4& boneWeights = animVertices[i].boneWeights;
			TYfloat totalWeight = boneWeights.x + boneWeights.y + boneWeights.z + boneWeights.w;
			if (totalWeight > 0.0f)
			{
				animVertices[i].boneWeights = TYvec4(
					boneWeights.x / totalWeight,
					boneWeights.y / totalWeight,
					boneWeights.z / totalWeight,
					boneWeights.w / totalWeight
				);
			}
		}
	}

	for (TYuint i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace& face = mesh->mFaces[i];
		for (TYuint j = 0; j < face.mNumIndices; j++)
		{
			Indices.push_back(face.mIndices[j]);
		}
	}

	for (TYsizet i = 0; i < Indices.size(); i += 3)
	{
		TYuint i1 = Indices[i];
		TYuint i2 = Indices[i + 1];
		TYuint i3 = Indices[i + 2];

		if (hasAnimations)
		{
			/*VertexAnim v1 = animVertices[i1];
			VertexAnim v2 = animVertices[i2];
			VertexAnim v3 = animVertices[i3];

			triangles.push_back(Triangle(v1, v2, v3));*/
			Vertex v1 = animVertices[i1];
			Vertex v2 = animVertices[i2];
			Vertex v3 = animVertices[i3];

			triangles.push_back(Triangle(v1, v2, v3));
		}
		else
		{
			Vertex v1 = Vertices[i1];
			Vertex v2 = Vertices[i2];
			Vertex v3 = Vertices[i3];

			triangles.push_back(Triangle(v1, v2, v3));
		}
	}
}

TYvoid Model::AddTriangles(TYvector<Triangle>& pTriangles)
{
	triangles.insert(triangles.end(), pTriangles.begin(), pTriangles.end());
}

TYvoid Model::GenOctree()
{
	octree->GlobalTriangles.insert(octree->GlobalTriangles.begin(), triangles.begin(), triangles.end());
	octree->Make();
}

TYmat Model::GetMatrix()
{
	TYmat model = TYmat(1.0f);
	//model = glm::translate(model, center);
	model = glm::scale(model, { 1.0f, 1.0f, 1.0f });

	return model;
}

MeshHandle Model::GenHandle_GL()
{
	MeshHandle mh;

	TYuint VBO, IBO;

	glGenVertexArrays(1, &mh.VAO);
	glBindVertexArray(mh.VAO);

	glGenBuffers(1, &VBO);
	glGenBuffers(1, &IBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	if (hasAnimations)
	{
		glBufferData(GL_ARRAY_BUFFER, animVertices.size() * sizeof(VertexAnim), &animVertices[0], GL_STATIC_DRAW);
	}
	else
	{
		glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(Vertex), &Vertices[0], GL_STATIC_DRAW);
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.size() * sizeof(TYuint), &Indices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	if (hasAnimations)
	{
		glEnableVertexAttribArray(3);
		glEnableVertexAttribArray(4);
	}

	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(TYfloat), (TYvoid*)0);
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(TYfloat), (TYvoid*)(3 * sizeof(TYfloat)));
	//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(TYfloat), (TYvoid*)(6 * sizeof(TYfloat)));

	if (hasAnimations)
	{
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAnim), (GLvoid*)offsetof(VertexAnim, position));
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAnim), (GLvoid*)offsetof(VertexAnim, normal));
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexAnim), (GLvoid*)offsetof(VertexAnim, texCoord));
		glVertexAttribIPointer(3, 4, GL_INT, sizeof(VertexAnim), (GLvoid*)offsetof(VertexAnim, boneIds));
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(VertexAnim), (GLvoid*)offsetof(VertexAnim, boneWeights));
	}
	else
	{
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, position));
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, normal));
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, texCoord));
	}

	glBindVertexArray(0);

	mh.indexCount = Indices.size();

	meshHandle = mh;

	return meshHandle;
}

Model::~Model()
{

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