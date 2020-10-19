#pragma once

#include <algorithm>

#ifndef TYAMOE3D

#include "Types.h"

#else

#include "Tyamoe3DHelper.h"
#include EngineInc(Types.h)

#endif // TYAMOE3D

#include "GenericDraw.h"
#include "Utils.h"

class BoundingVolume
{
public:
	BoundingVolume() {}
	virtual ~BoundingVolume() {}

	virtual TYbool Intersect(TYvec rayOrig, TYvec rayDir) { return false; }
	virtual TYvoid Draw() { return; }
};

class BoundingSphere : public BoundingVolume
{
public:
	BoundingSphere(TYvec Min = {}, TYvec Max = {}) : BoundingVolume()
	{
		TYvec rad = Max - Min / 2.0f;
		radius = TyMax({ rad.x, rad.y, rad.z });
		center = (Max + Min) / 2.0f;
	}

	TYbool Intersect(TYvec rayOrig, TYvec rayDir) 
	{
		TYvec l = center - rayOrig;

		TYfloat tca = glm::dot(l, rayDir);

		if (tca < 0)
			return false;

		TYfloat d2 = glm::dot(l, l) - tca * tca;

		if (d2 > radius * radius)
			return false;

		return true;
	}

	TYvoid Draw() 
	{
		GenericDraw::DrawSphere(center, radius, TYvec(0.0f, 1.0f, 0.0f));
		return; 
	}

	TYvec center;
	TYfloat radius;

	BoundingVolume* left = TYnull;
	BoundingVolume* right = TYnull;
};

class BoundingBox : public BoundingVolume
{
public:
	TYbool Intersect(TYvec rayOrig, TYvec rayDir)
	{
		TYfloat tmin = std::numeric_limits<TYfloat>::max();
		TYfloat tmax = -std::numeric_limits<TYfloat>::max();

		for (TYint a = 0; a < 3; ++a) 
		{
			TYfloat invD = 1.0f / rayDir[a];
			TYfloat t0 = (min[a] - rayOrig[a]) * invD;
			TYfloat t1 = (max[a] - rayOrig[a]) * invD;
			if (invD < 0.0f) 
			{
				TYfloat temp = t1;
				t1 = t0;
				t0 = temp;
			}

			tmin = t0 > tmin ? t0 : tmin;
			tmax = t1 < tmax ? t1 : tmax;

			if (tmax <= tmin)
				return false;
		}

		return true;
	}

	TYvoid Draw()
	{
		GenericDraw::DrawCube(center, extents, TYvec(1.0f, 0.0f, 0.0f), 2.0f);
		return;
	}

	TYvec center;
	TYvec extents;
	TYvec min;
	TYvec max;

	BoundingVolume* left = TYnull;
	BoundingVolume* right = TYnull;
};

typedef class BoundingVolume* BoundingVolumePtr;