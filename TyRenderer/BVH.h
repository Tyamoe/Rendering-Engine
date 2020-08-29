#pragma once


#ifndef TYAMOE3D

#include "Types.h"

#else

#include "Tyamoe3DHelper.h"
#include EngineInc(Types.h)

#endif // TYAMOE3D

#include "BoundingVolume.h"
#include "Geometry.h"

class BVH
{
public:
	BoundingVolumePtr head;

	static BVH* GenerateBVH(Sphere* sphere);
	static BVH* GenerateBVH(Triangle* triangle);
	static BVH* GenerateBVH(Model* model);
private:

};
