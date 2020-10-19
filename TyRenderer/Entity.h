#pragma once

#include "Types.h"

#include "Utils.h"

class Transform;
class Animation;
class Mesh;

enum class Component
{
	Transform,
	Mesh,
	Animation,
};

class Entity
{
	public:
		Entity(TYstring name_ = "");
		~Entity();

		TYstring GetName();

		TYmat GetMatrix();

	private:
		TYstring name = "";
		TYint hs_name;

		Transform* transform;
		Mesh* mesh;
		Animation* animation;

// Templated
public:
	template<Component e>
	TYvoid Set(TYvoid* component)
	{
		if (e == Component::Transform)
		{
			transform = reinterpret_cast<Transform*>(component);
		}
		else if (e == Component::Mesh)
		{
			mesh = reinterpret_cast<Mesh*>(component);
		}
		else if (e == Component::Animation)
		{
			animation = reinterpret_cast<Animation*>(component);
		}
	}

	template<typename t>
	t Get(t p = 0)
	{
		return t();
	}

	template<>
	Transform* Get(Transform* t)
	{
		return transform;
	}

	template<>
	Mesh* Get(Mesh* p)
	{
		return mesh;
	}

	template<>
	Animation* Get(Animation* p)
	{
		return animation;
	}

// Static
public:
	static Entity* GetEntity(TYstring name)
	{
		return EntityList[Hash(name)];
	}

	template<typename t>
	static t Get(TYstring name, t p = 0)
	{
		return t();
	}

	template<>
	static Transform* Get(TYstring name, Transform* p)
	{
		return GetEntity(name)->transform;
	}

	template<>
	static Mesh* Get(TYstring name, Mesh* p)
	{
		return GetEntity(name)->mesh;
	}

	template<>
	static Animation* Get(TYstring name, Animation* p)
	{
		return GetEntity(name)->animation;
	}

	static inline TYumap<TYint, Entity*> EntityList;
private:
};

typedef Entity* EntityPtr;