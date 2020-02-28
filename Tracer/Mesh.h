#pragma once

#include "Types.h"
//#include "Geometry.h"

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
		TYvectorUI indices;
		TYvector3 normals;
		TYvector<TYvec2> texcoords;

		//TYvector<Geometry> geometry;

		TYuint VAO;
		TYuint VBO;
		TYuint IBO;
};

typedef Mesh* MeshPtr;
