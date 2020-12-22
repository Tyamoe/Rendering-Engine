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

struct BoneInfo1;

class Animation
{
public:

	TYumap<TYstring, TYuint> boneMapping;
	TYvector<BoneInfo1> boneInfo;
	TYuint numBones;

	TYfloat timeElapsed = 0.05f;

	TYuint frameCount = 0;

	TYfloat duration = 0.0f;
	TYfloat ticksPerSecond = 1.0f;

	TYmat globalInvMatrix;

	aiScene* mScene;

	TYvector<TYmat4> currentPose = {};

	//////////////////////////////////////

	Animation();
	~Animation();
	
	TYvoid Update(TYfloat dt);

	TYvoid SetFrame(TYfloat frame);
	TYvoid SetFrame(TYint frame);

	Skeleton* skeleton;

	TYvector<BoneKeyframes*> skeleton1;

	TYvoid CreateSkeleton(const aiScene* scene);
	TYvoid LoadKeyframes(TYumap<TYint, BoneKeyframes*>&);

	TYvoid UpdatePose();
	TYvoid UpdatePose(const aiNode* pNode, const TYmat& ParentTransform);

	friend class Mesh;

private:
	TYbool globalKeyframes = false;

	const aiNodeAnim* GetNodeAnim(const aiAnimation* pAnimation, TYstring& NodeName);

	TYvec GetPosePosition(const aiNodeAnim* pNodeAnim);
	TYvec GetPoseScale(const aiNodeAnim* pNodeAnim);
	TYquaternion GetPoseRotation(const aiNodeAnim* pNodeAnim);

	TYvoid GenGlobalKeyframes();

private:
	static Animation* CreateAnimation(const aiScene* scene);

};