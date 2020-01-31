#pragma once

#include "Types.h"

class Mesh
{
	public:
		Mesh();
		~Mesh();

		void CreateBuffers();
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