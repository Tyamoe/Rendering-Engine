///////////////////////////////////////////////////////////////////////////////////////////////
// Handles animation updates
// Stores current animation, (optional) Animator 
///////////////////////////////////////////////////////////////////////////////////////////////

#include "Types.h"

class Mesh;
class Animator;
struct Bone;

struct aiScene;
struct aiNodeAnim;
struct aiNode;
struct aiAnimation;
struct aiMesh;

class Animation
{
public:
	friend class Mesh;

	Animation();
	~Animation();
	
	TYvoid Update(TYfloat dt);

	TYvoid UpdatePose();
	TYvoid UpdatePose(const aiNode* pNode, const TYmat& ParentTransform);

	TYvoid DrawSkeleton(const TYmat& modelMatrix);

	TYvoid SetFrame(TYfloat frame);
	TYvoid SetFrame(TYint frame);

	aiScene* mScene;

	TYmat globalInvMatrix;

	TYuint numBones = 0;
	TYuint frameCount = 0;

	TYfloat duration = 0.0f;
	TYfloat ticksPerSecond = 1.0f;

	TYfloat timeElapsed = 0.0f;

	TYvector<TYmat4> currentPose = {};

	TYvector<Bone> bones;
	TYumap<TYint, TYuint> boneMapping;
	TYumap<TYint, aiNodeAnim*> nodeMapping;

private:
	TYvec GetPosePosition(const aiNodeAnim* pNodeAnim);
	TYvec GetPoseScale(const aiNodeAnim* pNodeAnim);
	TYquaternion GetPoseRotation(const aiNodeAnim* pNodeAnim);

private:
	static Animation* CreateAnimation(const aiScene* scene);

};