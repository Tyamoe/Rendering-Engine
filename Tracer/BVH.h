#pragma once

#include "Types.h"
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
