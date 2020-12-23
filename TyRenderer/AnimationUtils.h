#pragma once

#include "Types.h"

struct Bone
{
	TYmat BoneOffset;
	TYmat BoneTransform;

	TYmat GlobalTransform;

	Bone()
	{
		BoneOffset = TYmat(1.0f);
		BoneTransform = TYmat(1.0f);
		GlobalTransform = TYmat(1.0f);
	}
};
