#include "Transform.h"

Transform::Transform(TYvec pos_, TYvec scale_, TYquaternion rot)
{
	position = pos_;
	scale = scale_;
	rotation = rot;
}

Transform::~Transform()
{

}

TYmat Transform::GetMatrix()
{
	if (dirty)
	{
		TYmat t = glm::translate(TYmat(1.0f), position);
		TYmat r = glm::mat4_cast(rotation);
		TYmat s = glm::scale(TYmat(1.0f), scale);

		model = t * r * s;

		dirty = false;
	}

	return model;
}