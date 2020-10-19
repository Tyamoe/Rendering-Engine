#pragma once

#include "Types.h"

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/vector3.h>
#include <assimp/Importer.hpp>

inline TYmat AssimpToGlm(aiMatrix4x4 mat)
{
	TYmat m;
	for (TYint y = 0; y < 4; y++)
	{
		for (TYint x = 0; x < 4; x++)
		{
			m[x][y] = mat[y][x];
		}
	}
	return m;
}

inline TYvec AssimpToGlm(aiVector3D vec)
{
	return TYvec(vec.x, vec.y, vec.z);
}

inline TYvec4 AssimpToGlm(aiColor4D vec)
{
	return TYvec4(vec.r, vec.g, vec.b, vec.a);
}

inline TYvec AssimpToGlm(aiColor3D vec)
{
	return TYvec(vec.r, vec.g, vec.b);
}

inline TYquaternion AssimpToGlm(aiQuaternion quat)
{
	TYquaternion q;
	q.x = quat.x;
	q.y = quat.y;
	q.z = quat.z;
	q.w = quat.w;

	return q;
}
