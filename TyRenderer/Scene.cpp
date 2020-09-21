#include "stdafx.h"

#include "Scene.h"

Scene::Scene()
{
	// light
	AddGeometry(new Sphere(TYvec(0.0, 30, 0), 2.0f, PixelColorF(0.65f, 0.95f, 0.1f), 0.0f, 0.0f, PixelColorF(1.0f, 1.0f, 0.3f)));

	// position, radius, surface color, reflectivity, transparency, emission color
	AddGeometry(new Sphere(TYvec(0.0, -10007, -20), 10000.0f, PixelColorF(0.20f, 0.20f, 0.20f), 0.7f, 0.0f));
	//AddGeometry(new Sphere(TYvec(0.0, -1, -20), 4.0f, PixelColorF(1.0f, 0.32f, 0.36f), 1.0f, 0.5));
	//AddGeometry(new Sphere(TYvec(0, 0, 0), 2.0f, PixelColorF(0.90f, 0.82f, 0.36f), 0.0f, 0.0f));
	AddGeometry(new Sphere(TYvec(5.0, -1, -25), 3.0f, PixelColorF(0.2f, 0.2f, 0.97f), 0.0f, 0.0f));
	//AddGeometry(new Sphere(TYvec(-5.5, -2, -15), 3.0f, PixelColorF(0.30f, 0.90f, 0.50f), 0.0f, 1.0f));
	//AddGeometry(new Sphere(TYvec(15.0, -1, -25), 3.0f, PixelColorF(0.97f, 0.27f, 0.97f), 0.0f, 0.0f));
	//AddGeometry(new Sphere(TYvec(-5.5, -3, -30), 1.0f, PixelColorF(0.30f, 0.90f, 0.90f), 1.0f, 0.0f));
	
	//AddGeometry(new Triangle(TYvec(0, 0, 0), Vertex(TYvec(10.5f, -10.5f, -10.5f)), Vertex(TYvec(10.5f, 10.5f, -10.5f)), Vertex(TYvec(-10.5f, -10.5f, -10.5f)), PixelColorF(0.10f, 0.90f, 0.10f), 0.0f, 0.0f));
	AddGeometry(new Triangle(TYvec(0, 0, 0), Vertex(TYvec(-5.5f, 5.5f, -5.5f)), Vertex(TYvec(-5.5f, -5.5f, -5.5f)), Vertex(TYvec(5.5f, 5.5f, -5.5f)), PixelColorF(0.10f, 0.90f, 0.10f), 0.0f, 0.0f));
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
	//
	/*Model* m = new Model("./resources/models/cup.obj", PixelColorF(0.64f, 0.0f, 0.64f), 0.0f, 0.0f, PixelColorF(), TYvec(0.0f, 0.0f, -15.0f));
	geometry.push_back(m); 
	BVH* bvh = BVH::GenerateBVH(m);
	
	m->bvh = bvh;*/

	/*m = new Model("./resources/models/bunny.obj", PixelColorF(0.64f, 0.0f, 0.64f), 0.0f, 0.0f, PixelColorF(), TYvec(-10.0f, 5.0f, -15.0f));
	geometry.push_back(m);*/

	//AddGeometry(Make<Sphere>(TYvec(5.0f, -1.0f, -25.0f), 4.2f));
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
	for (Geometry* geo : geometry)
	{
		geo->GenHandle_GL();
	}
}

TYvoid Scene::GenOctree()
{
	for (Geometry* geo : geometry)
	{
		if (geo->GetType() == geoModel)
		{
			static_cast<Model*>(geo)->GenOctree();
		}
	}
}

Scene::~Scene()
{

}

TYvoid Scene::Init()
{

}