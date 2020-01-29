#pragma once

#include "Types.h"

#include "Shape.h"
#include "Transform.h"

class Entity
{
	public:
		Entity();
		~Entity();

	private:
		TYstring name;

		TransformPtr transform;
		ShapePtr shape;
};

typedef Entity* EntityPtr;