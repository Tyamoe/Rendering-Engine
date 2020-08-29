#pragma once

#ifndef TYAMOE3D

#include "Types.h"

#else

#include "Tyamoe3DHelper.h"
#include EngineInc(Types.h)

#endif // TYAMOE3D

class Material
{
	public:
		Material();
		~Material();

	private:
		TYvec4 color;

};

typedef Material* MaterialPtr;