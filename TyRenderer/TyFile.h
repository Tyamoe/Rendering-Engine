#pragma once

#include "Types.h"

#define MESH 0
#define IMAGE 1
#define DATA 2

TYvector<TYset<TYstring>> fileType =
{
	{"obj","fbx","dae","stl","ply","blend","3ds","x","xml","mdl","smd","vta","3d"},

	{"bmp","png","jpg","tga","dds","psd","hdr","gif","pic"},

	{"json","txt",""},
};

template<typename T>
class TyFile
{
public:
	TyFile(TYstring filename);

private:
	TYstring fileName;
	TYstring fileType;

	T* data;
};