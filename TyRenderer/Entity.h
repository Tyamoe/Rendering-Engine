#pragma once

#include "Types.h"

#include "Shape.h"
#include "Transform.h"
#include "Physics.h"

class Entity
{
	public:
		Entity();
		~Entity();

	private:
		TYstring name;

		TransformPtr transform;
		PhysicsPtr physics;
		ShapePtr shape;
};

typedef Entity* EntityPtr;