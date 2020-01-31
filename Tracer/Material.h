#pragma once

#include "Types.h"

class Material
{
	public:
		Material();
		~Material();

	private:
		TYvec4 color;

};

typedef Material* MaterialPtr;