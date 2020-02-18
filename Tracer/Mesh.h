#pragma once

#include "Types.h"

class Triangle
{
	TYvector3 vertices;
	TYvector3 normals;
	TYvector<TYvec2> texcoords;
};

class Mesh
{
	public:
		Mesh();
		~Mesh();

		TYvoid CreateBuffers();
		Mesh& operator+=(Mesh& m);

		TYuint size() { return (TYuint)indices.size(); }

	private:
		TYvector3 vertices;
		TYvectorui indices;
		TYvector3 normals;
		TYvector<TYvec2> texcoords;

		TYuint VAO;
		TYuint VBO;
		TYuint IBO;
};

typedef Mesh* MeshPtr;
