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
	friend class Mesh;

	Animation();
	~Animation();
	
	TYvoid Update(TYfloat dt);

	TYvoid UpdatePose();
	TYvoid UpdatePose(const aiNode* pNode, const TYmat& ParentTransform);

	TYvoid DrawSkeleton();

	TYvoid SetFrame(TYfloat frame);
	TYvoid SetFrame(TYint frame);
	
	TYumap<TYint, aiNodeAnim*> nodeMapping;
	TYumap<TYint, TYuint> boneMapping;
	TYvector<BoneInfo1> boneInfo;

	TYuint numBones = 0;

	TYfloat timeElapsed = 0.05f;

	TYuint frameCount = 0;

	TYfloat duration = 0.0f;
	TYfloat ticksPerSecond = 1.0f;

	TYmat globalInvMatrix;

	aiScene* mScene;

	TYvector<TYmat4> currentPose = {};

private:
	const aiNodeAnim* GetNodeAnim(const aiAnimation* pAnimation, TYstring& NodeName);

	TYvec GetPosePosition(const aiNodeAnim* pNodeAnim);
	TYvec GetPoseScale(const aiNodeAnim* pNodeAnim);
	TYquaternion GetPoseRotation(const aiNodeAnim* pNodeAnim);

	void CalcInterpolatedScaling(TYvec& Out, const aiNodeAnim* pNodeAnim);
	void CalcInterpolatedRotation(TYquaternion& Out, const aiNodeAnim* pNodeAnim);
	void CalcInterpolatedPosition(TYvec& Out, const aiNodeAnim* pNodeAnim);
	TYuint FindScaling(const aiNodeAnim* pNodeAnim);
	TYuint FindRotation(const aiNodeAnim* pNodeAnim);
	TYuint FindPosition(const aiNodeAnim* pNodeAnim);
private:
	static Animation* CreateAnimation(const aiScene* scene);

};