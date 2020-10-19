#pragma once

#ifndef TYAMOE3D

#include "Types.h"

#else

#include "Tyamoe3DHelper.h"
#include EngineInc(Types.h)

#endif // TYAMOE3D

struct aiNode;
class Transform;

class Bone
{
public:
	Bone(TYstring n, Bone* p, TYint keyId, TYmat off = TYmat(1.0f));

	TYstring name = "";

	Transform* transform;

	Bone* parent;

	TYmat offset;
	TYmat local = TYmat(1.0f);

	TYvec globalPosition = TYvec();

	TYint keyframeId = 0;

private:
};

class BoneKeyframes
{
public:
	BoneKeyframes(Bone* b, TYuint ID = 0) : bone(b), id(ID) {}

	TYuint id = 0;
	Bone* bone;

	TYvectorF positionTimestamps = {};
	TYvectorF rotationTimestamps = {};
	TYvectorF scaleTimestamps = {};

	TYvector3 positions = {};
	TYvector<TYquaternion> rotations = {};
	TYvector3 scales = {};
};

class Skeleton
{
public:
	Skeleton(aiNode*, TYint, TYumap<TYint, TYmat>&);

	TYvoid GenSkeleton(aiNode*, Bone*, TYumap<TYint, TYmat>&, TYint&);

	TYstring name = "";

	TYvec center = {};

	TYint boneCount = 0;
	TYumap<TYint, BoneKeyframes*> skeleton;


private:
};
