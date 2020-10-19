#include "stdafx.h"

#include "Scene.h"
#include "Entity.h"

#include "Transform.h"
#include "Animation.h"
#include "Mesh.h"
#include "Material.h"

Scene::Scene(TYint i)
{
	if (i != 0)
	{
		TYbool hasAnim, hasBones;

		// light
		AddGeometry(new Sphere(TYvec(0.0, 30, 0), 2.0f, PixelColorF(0.65f, 0.95f, 0.1f), 0.0f, 0.0f, PixelColorF(1.0f, 1.0f, 0.3f)));

		Entity* entity = CreateEntity("TestRaytrace");
		entity->Set<Component::Mesh>(new Mesh(geometry[0], new Material(TYvec4(0.65f, 0.95f, 0.1f, 1.0f))));
		entity->Set<Component::Transform>(new Transform(TYvec(0.0f, 30.0f, 0.0f), TYvec(2.0f)));

		Entity* entity1 = CreateEntity("TestRaytrace2");
		TYpair<Mesh*, Animation*> mesh = Mesh::CreateMesh("./resources/models/cube2.obj", hasBones, hasAnim);
		entity1->Set<Component::Mesh>(mesh.first);
		/*if (hasAnim)
		{
			entity1->Set<cc::Animation>(mesh.second);
		}*/
		entity1->Set<Component::Transform>(new Transform(TYvec(0.0f, 0.0f, 0.0f), TYvec(1.0f)));

		entity1 = CreateEntity("TestRaytrace52");
		mesh = Mesh::CreateMesh("./resources/models/trofy.obj", hasBones, hasAnim);
		entity1->Set<Component::Mesh>(mesh.first);
		entity1->Set<Component::Transform>(new Transform(TYvec(7.0f, 0.0f, -5.0f), TYvec(1.0f)));

		entity1 = CreateEntity("TestRaytrace3");
		entity1->Set<Component::Mesh>(new Mesh(new Sphere(TYvec(0.0, -10007, -20), 10000.0f, PixelColorF(0.20f, 0.20f, 0.20f), 0.7f, 0.0f), new Material(TYvec4(0.65f, 0.95f, 0.1f, 1.0f))));
		entity1->Set<Component::Transform>(new Transform(TYvec(0.0f, 0.0f, 0.0f), TYvec(1.0f)));

		return;
	}
	// light
	AddGeometry(new Sphere(TYvec(0.0, 30, 0), 2.0f, PixelColorF(0.65f, 0.95f, 0.1f), 0.0f, 0.0f, PixelColorF(1.0f, 1.0f, 0.3f)));

	// position, radius, surface color, reflectivity, transparency, emission color
	//AddGeometry(new Sphere(TYvec(0.0, -10007, -20), 10000.0f, PixelColorF(0.20f, 0.20f, 0.20f), 0.7f, 0.0f));
	//AddGeometry(new Sphere(TYvec(0.0, -1, -20), 4.0f, PixelColorF(1.0f, 0.32f, 0.36f), 1.0f, 0.0));
	//AddGeometry(new Sphere(TYvec(0, 0, 0), 2.0f, PixelColorF(0.90f, 0.82f, 0.36f), 0.0f, 0.0f));
	//AddGeometry(new Sphere(TYvec(5.0, -1, -25), 3.0f, PixelColorF(0.2f, 0.2f, 0.97f), 0.0f, 0.0f));
	//AddGeometry(new Sphere(TYvec(-5.5, -2, -15), 3.0f, PixelColorF(0.30f, 0.90f, 0.50f), 0.0f, 1.0f));
	//AddGeometry(new Sphere(TYvec(15.0, -1, -25), 3.0f, PixelColorF(0.97f, 0.27f, 0.97f), 0.0f, 0.0f));
	//AddGeometry(new Sphere(TYvec(-5.5, -3, -30), 1.0f, PixelColorF(0.30f, 0.90f, 0.90f), 1.0f, 0.0f));
	
	//AddGeometry(new Triangle(TYvec(0, 0, 0), Vertex(TYvec(10.5f, -10.5f, -10.5f)), Vertex(TYvec(10.5f, 10.5f, -10.5f)), Vertex(TYvec(-10.5f, -10.5f, -10.5f)), PixelColorF(0.10f, 0.90f, 0.10f), 0.0f, 0.0f));
	//AddGeometry(new Triangle(TYvec(0, 0, 0), Vertex(TYvec(-5.5f, 5.5f, -5.5f)), Vertex(TYvec(-5.5f, -5.5f, -5.5f)), Vertex(TYvec(5.5f, 5.5f, -5.5f)), PixelColorF(0.10f, 0.90f, 0.10f), 0.0f, 0.0f));
	/*
	AddGeometry(new Triangle(TYvec(0, 0, 0), Vertex(TYvec(-0.5f, 0.5f, -0.5f)), Vertex(TYvec(-0.5f, 0.5f, 0.5f)), Vertex(TYvec(0.5f, 0.5f, 0.5f)), PixelColorF(0.10f, 0.90f, 0.10f), 0.0f, 0.0f));
	AddGeometry(new Triangle(TYvec(0, 0, 0), Vertex(TYvec(-0.5f, 0.5f, -0.5f)), Vertex(TYvec(0.5f, 0.5f, 0.5f)), Vertex(TYvec(-0.5f, 0.5f, 0.5f)), PixelColorF(0.10f, 0.90f, 0.10f), 0.0f, 0.0f));

	AddGeometry(new Triangle(TYvec(0, 0, 0), Vertex(TYvec(0.5f, 0.5f, 0.5f)), Vertex(TYvec(0.5f, -0.5f, 0.5f)), Vertex(TYvec(0.5f, -0.5f, -0.5f)), PixelColorF(0.10f, 0.90f, 0.10f), 0.0f, 0.0f));
	AddGeometry(new Triangle(TYvec(0, 0, 0), Vertex(TYvec(0.5f, 0.5f, 0.5f)), Vertex(TYvec(0.5f, -0.5f, -0.5f)), Vertex(TYvec(0.5f, 0.5f, -0.5f)), PixelColorF(0.10f, 0.90f, 0.10f), 0.0f, 0.0f));
	*/
	//AddGeometry(new Triangle(TYvec(-10.5, 3, -20), Vertex(TYvec(-2.6f, 5.4f, -20.0f)), Vertex(TYvec(0.01f, 2.39f, -17.0f)), Vertex(TYvec(2.6f, 0.728f, -21)), PixelColorF(0.10f, 0.90f, 0.10f), 0.0f, 0.0f));
	//AddGeometry(new Triangle(TYvec(10.5, -3, -22), Vertex(TYvec(10.5, 4, -30)), Vertex(TYvec(9.0f, 1, -22)), Vertex(TYvec(12.0f, 1, -22)), PixelColorF(0.10f, 0.10f, 0.90f), 0.0f, 0.0f));
	//AddGeometry(new Triangle(TYvec(0.5, -3, -22), Vertex(TYvec(0.5, 5, -20)), Vertex(TYvec(3.0f, 0, -21)), Vertex(TYvec(-2.0f, 1, -19)), PixelColorF(0.90f, 0.10f, 0.10f), 0.1f, 0.0f));
	//AddGeometry(new Triangle(TYvec(11.5, -2, -15), Vertex(TYvec(11.15, 3, -15)), Vertex(TYvec(16.0f, -2, -10)), Vertex(TYvec(10.0f, -2, -10)), PixelColorF(0.90f, 0.90f, 0.10f), 0.0f, 0.0f));
	
	/*Model* m = new Model("./resources/models/cube2.obj", PixelColorF(0.64f, 0.64f, 0.64f), 0.0f, 0.0f, PixelColorF(), TYvec(0.0f, 0.0f, -15.0f));
	geometry.push_back(m); 
	BVH* bvh = BVH::GenerateBVH(m);
	
	m->bvh = bvh;*/

	/*m = new Model("./resources/models/bunny.obj", PixelColorF(0.64f, 0.0f, 0.64f), 0.0f, 0.0f, PixelColorF(), TYvec(-10.0f, 5.0f, -15.0f));
	geometry.push_back(m);*/

	//AddGeometry(Make<Sphere>(TYvec(5.0f, -1.0f, -25.0f), 4.2f));

	
	Entity* entity = CreateEntity("Test");
	entity->Set<Component::Mesh>(new Mesh(geometry[0], new Material(TYvec4(0.65f, 0.95f, 0.1f, 1.0f))));
	entity->Set<Component::Transform>(new Transform(TYvec(0.0f, 30.0f, 0.0f), TYvec(2.0f)));

	TYbool hasAnim, hasBones;

	// Entity Test
	{
		Entity* entity1 = CreateEntity("Test1");

		TYpair<Mesh*, Animation*> mesh = Mesh::CreateMesh("./resources/models/Cinema4D.fbx", hasBones, hasAnim);
		//TYpair<Mesh*, Animation*> mesh = Mesh::CreateMesh("./resources/models/NewCharAnim_WalkGun.fbx", hasBones, hasAnim);
		//TYpair<Mesh*, Animation*> mesh = Mesh::CreateMesh("./resources/models/walking.dae", hasBones, hasAnim);
		//TYpair<Mesh*, Animation*> mesh = Mesh::CreateMesh("./resources/models/TestFBX.fbx", hasBones, hasAnim);
		using cc = Component;
		entity1->Set<cc::Mesh>(mesh.first);
		if (hasAnim)
		{
			entity1->Set<cc::Animation>(mesh.second);
		}
		entity1->Set<cc::Transform>(new Transform(TYvec(0.0f, 0.0f, 0.0f), TYvec(1.0f)));
	}
	/*Entity* entity1;
	for (int i = 0; i < 50; i++)
	{
		TYvec pos = { GetRand(100.0f, 270.0f), GetRand(10.0f, 65.0f), GetRand(-160.0f, 160.0f) };
		TYvec4 sss = { GetRand(0.0f, 1.0f), GetRand(0.0f, 1.0f), GetRand(0.0f, 1.0f), 1.0f };

		entity1 = CreateEntity("Test1" + std::to_string(i));
		using cc = Component;
		entity1->Set<cc::Mesh>(new Mesh(geometry[0], (i % 2 == 0 ? new Material(sss) : new Material(MaterialShader::PBR) )));
		if (hasAnim)
		{
			//entity1->Set<cc::Animation>(mesh.second);
		}
		entity1->Set<cc::Transform>(new Transform(pos, TYvec(GetRand(1.0f, 10.0f))));
	}*/

	// Entity Test 2
	{
		/*Entity* entity2 = CreateEntity("Test2");

		TYpair<Mesh*, Animation*> mesh2 = Mesh::CreateMesh("./resources/models/maya.fbx", hasBones, hasAnim);

		entity2->Set<Component::Mesh>(mesh2.first);
		if (hasAnim)
		{
			entity2->Set<Component::Animation>(mesh2.second);
		}
		entity2->Set<Component::Transform>(new Transform(TYvec(0.0f, 0.0f, 0.0f), TYvec(0.5f)));*/
	}

	// Entity Test 3
	{
		/*Entity* entity3 = CreateEntity("Test3");

		TYpair<Mesh*, Animation*> mesh23 = Mesh::CreateMesh("./resources/models/nuketown.fbx", hasBones, hasAnim, new Material(MaterialShader::PBR));

		entity3->Set<Component::Mesh>(mesh23.first);
		if (hasAnim)
		{
			entity3->Set<Component::Animation>(mesh23.second);
		}
		entity3->Set<Component::Transform>(new Transform(TYvec(280.0f, 0.0f, 0.0f), TYvec(0.5f)));*/
	}
}

Entity* Scene::CreateEntity(TYstring name)
{
	Entity* entity = new Entity(name);
	entityList.push_back(entity);

	return entity;
}

TYvoid Scene::AddGeometry(Geometry* geo)
{
	if (geo)
	{
		geometry.push_back(geo);
	}
}

TYvoid Scene::AddGeometry(TYpair<Geometry*, BVH*> geo)
{
	if (geo.first)
	{
		geometry.push_back(geo.first);
	}
	if (geo.second)
	{
		bvh.push_back(geo.second);
	}
}

TYvoid Scene::GenHandles_GL()
{
	for (Entity* entity : entityList)
	{
		entity->Get<MeshPtr>()->GenHandle_GL();
	}
}

TYvoid Scene::GenOctree()
{
	for (Entity* entity : entityList)
	{
		entity->Get<MeshPtr>()->GenOctree();
	}
}

Scene::~Scene()
{

}

TYvoid Scene::Init()
{

}