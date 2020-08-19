#pragma once

#include "Types.h"

class Physics
{
public:
	Physics();
	~Physics();

private:
	TYfloat mass;
	TYvec velocity;
	TYvec acceleration;
};

typedef Physics* PhysicsPtr;