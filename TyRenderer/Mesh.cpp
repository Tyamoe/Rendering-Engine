#include "Tyamoe3DHelper.h"

#ifndef TYAMOE3D

#include "stdafx.h"

#else

#include EngineInc(stdafx.h)

#endif // TYAMOE3D

#include "Mesh.h"

Mesh::Mesh()
{

}

Mesh::~Mesh()
{

}

void Mesh::CreateBuffers()
{

}

Mesh& Mesh::operator+=(Mesh& m)
{
	return *this;
}