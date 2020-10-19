#include "Tyamoe3DHelper.h"

#ifndef TYAMOE3D

#include "stdafx.h"

#else

#include EngineInc(stdafx.h)

#endif // TYAMOE3D

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

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
	TYmat t = glm::translate(TYmat(1.0f), position);
	//TYmat r = glm::toMat4(TYquaternion(glm::radians(euler)));
	TYmat r = glm::toMat4(rotation);
	//TYmat r = TYmat(1.0f);//
	//r = glm::rotate(r, glm::radians(euler.x), TYvec(1, 0, 0));
	//r = glm::rotate(r, glm::radians(euler.y), TYvec(0, 1, 0));
	//r = glm::rotate(r, glm::radians(euler.z), TYvec(0, 0, 1));
	TYmat s = glm::scale(TYmat(1.0f), scale);

	model = t * r * s;

	return model;
}