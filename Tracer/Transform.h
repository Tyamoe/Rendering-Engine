#pragma once

#include "Types.h"

class Transform
{
	public:
		Transform();
		~Transform();

	private:
		TYvec position;
		TYvec scale;
		TYvec rotation;
		TYquaternion quat;// = tquat(vec3(0));
};

typedef Transform* TransformPtr;