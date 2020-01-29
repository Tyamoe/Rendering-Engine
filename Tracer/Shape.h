#pragma once

#include "Types.h"

#include "Material.h"

class Shape
{
	public:
		Shape();
		~Shape();

	private:
		MaterialPtr material;

		TYuint offset;
		TYuint length;
};

typedef Shape* ShapePtr;