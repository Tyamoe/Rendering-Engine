#pragma once

#include "Types.h"

struct aiNodeAnim;
struct aiNode;
class Transform;

struct BoneInfo1
{
	TYmat BoneOffset;
	TYmat FinalTransformation;
	TYmat GlobalTransformation;

	BoneInfo1()
	{
		BoneOffset = TYmat(1.0f);
		FinalTransformation = TYmat(1.0f);
		GlobalTransformation = TYmat(1.0f);
	}
};
