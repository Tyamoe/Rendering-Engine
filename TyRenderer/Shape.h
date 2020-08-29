#pragma once

#ifndef TYAMOE3D

#include "Types.h"

#else

#include "Tyamoe3DHelper.h"
#include EngineInc(Types.h)

#endif // TYAMOE3D

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