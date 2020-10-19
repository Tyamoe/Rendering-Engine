#include "stdafx.h"

#include "Entity.h"
#include "Utils.h"

#include "Transform.h"
#include "Animation.h"
#include "Mesh.h"


Entity::Entity(TYstring name_)
{
	if (name_ == "")
	{
		name_ = std::to_string(GetRand<TYint>(1000, 9999));
	}

	name = name_;
	hs_name = Hash(name);

	EntityList[hs_name] = this;
}

Entity::~Entity()
{

}

TYstring Entity::GetName()
{
	return name;
}

TYmat Entity::GetMatrix()
{
	return transform->GetMatrix();;
}