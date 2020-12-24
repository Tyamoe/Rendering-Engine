#pragma once

#include "Types.h"

enum class Transformation
{
	Position,
	Scale,
	Rotation,
};

class Transform
{
	public:
		Transform(TYvec pos = TYvec(), TYvec scale = TYvec(1.0f), TYquaternion rot = TYquaternion(TYvec4(0, 0, 0, 1)));
		~Transform();

		template<Transformation e>
		TYvoid Set(TYvec value)
		{
			if constexpr(e == Transformation::Position)
			{
				position = value;
			}
			else if constexpr (e == Transformation::Scale)
			{
				scale = value;
			}
			else if constexpr (e == Transformation::Rotation)
			{
				rotation = TYquaternion(value);
			}
			dirty = true;
		}

		TYvoid Set(TYquaternion value)
		{
			rotation = value;
			dirty = true;
		}

		template<Transformation e>
		TYvec Get()
		{
			if constexpr (e == Transformation::Position)
			{
				return position;
			}
			else if constexpr (e == Transformation::Scale)
			{
				return scale;
			}
		}

		TYquaternion GetRotation()
		{
			return rotation;
		}

		TYmat GetMatrix();

	private:
		TYvec position;
		TYvec scale;
		TYquaternion rotation;

		TYvec pivot = {};

		TYmat model;

		TYbool dirty = true;
};