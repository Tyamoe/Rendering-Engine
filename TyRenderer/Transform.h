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
				euler = value;
				rotation = glm::radians(value);
			}
		}

		TYvoid Set(TYquaternion value)
		{
			rotation = value;

			//printf("Set: %.3f, %.3f, %.3f \n", value.x, value.y, value.z);
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
			else if constexpr (e == Transformation::Rotation)
			{
				return euler;
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
		TYvec euler = TYvec();
		TYmat rotMat;

		TYvec pivot = {};

		TYmat model;
};