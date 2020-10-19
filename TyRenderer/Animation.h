///////////////////////////////////////////////////////////////////////////////////////////////
// Handles animation updates
// Stores current animation, (optional) Animator 
///////////////////////////////////////////////////////////////////////////////////////////////

#include "Types.h"

class Mesh;
class Bone;
class Animator;
class BoneKeyframes;

class Skeleton;

struct aiScene;
struct aiNodeAnim;
struct aiNode;
struct aiAnimation;
struct aiMesh;

class Animation
{
public:
	Animation();
	
	TYvoid Update(TYfloat dt);

	TYvoid SetFrame(TYfloat frame);
	TYvoid SetFrame(TYint frame);

	TYbool fbx = false;

	TYfloat timeElapsed = 0.05f;

	TYuint frameCount = 0;

	TYfloat duration = 0.0f;
	TYfloat ticksPerSecond = 1.0f;

	TYmat globalInvMatrix;

	aiScene* mScene;

	Skeleton* skeleton;

	TYvector<BoneKeyframes*> skeleton1;

	TYvector<TYmat4> currPose = {};

	TYvoid CreateSkeleton(const aiScene* scene);
	TYvoid LoadKeyframes(TYumap<TYint, BoneKeyframes*>&);

	TYvoid UpdatePose();

	friend class Mesh;

private:
	TYbool globalKeyframes = false;

	TYvec GetPosePosition(TYvectorF& timestamps, TYvector3& positions);
	TYvec GetPoseScale(TYvectorF& timestamps, TYvector3& scales);
	TYquaternion GetPoseRotation(TYvectorF& timestamps, TYvector<TYquaternion>& rotations);

	TYvoid GenGlobalKeyframes();

private:
	static Animation* CreateAnimation(const aiScene* scene, TYbool b = false);
};

typedef Animation* AnimationPtr;