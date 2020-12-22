#pragma once

class Sphere;
class Triangle;
class Model;
class BoundingVolume;

class BVH
{
public:
	BoundingVolume* head;

	~BVH();

	static BVH* GenerateBVH(Sphere* sphere);
	static BVH* GenerateBVH(Triangle* triangle);
	static BVH* GenerateBVH(Model* model);
private:

};
