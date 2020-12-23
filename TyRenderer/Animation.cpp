///////////////////////////////////////////////////////////////////////////////////////////////
// Handles animation updates
// Stores current animation, (optional) Animator 
///////////////////////////////////////////////////////////////////////////////////////////////

#include "Animation.h"

#include <SOIL2.h>
#include <deque>

#include "Transform.h"
#include "Animator.h"

#include "GenericDraw.h"

#include "AnimationUtils.h"
#include "MeshUtils.h"
#include "AssimpUtils.h"

#include "Utils.h"

Animation::Animation()
{

}

Animation::~Animation()
{

}

Animation* Animation::CreateAnimation(const aiScene* scene)
{
	Animation* animation = new Animation();

	aiAnimation* anim = scene->mAnimations[0];

	if (anim->mTicksPerSecond != 0.0f)
		animation->ticksPerSecond = (TYfloat)anim->mTicksPerSecond;
	else
		animation->ticksPerSecond = 1.0f;

	animation->duration = (TYfloat)anim->mDuration * (TYfloat)anim->mTicksPerSecond;
	animation->duration = (TYfloat)anim->mDuration;

	for (TYuint i = 0; i < anim->mNumChannels; i++)
	{
		const aiNodeAnim* pNodeAnim = anim->mChannels[i];
		TYstring NodeName(pNodeAnim->mNodeName.data);
		TYint hashStr = Hash(pNodeAnim->mNodeName.C_Str(), pNodeAnim->mNodeName.length);
		animation->nodeMapping.insert({ hashStr, (aiNodeAnim*)pNodeAnim });
	}

	return animation;
}

TYvoid Animation::SetFrame(TYfloat frame)
{
	timeElapsed = duration * frame;
}

TYvoid Animation::SetFrame(TYint frame)
{
	timeElapsed = duration * (frame / (TYfloat)frameCount);
}

TYvoid Animation::Update(TYfloat dt)
{
	timeElapsed += dt * ticksPerSecond;
	timeElapsed = (timeElapsed >= duration ? 0.0f : timeElapsed);
}

TYvoid Animation::UpdatePose()
{
	TYmat Identity(1.0f);

	UpdatePose(mScene->mRootNode, Identity);

	for (TYuint i = 0; i < numBones; i++)
	{
		currentPose[i] = bones[i].BoneTransform;
	}
}

TYvoid Animation::UpdatePose(const aiNode* pNode, const TYmat& ParentTransform)
{
	TYint hashStr = Hash(pNode->mName.C_Str(), pNode->mName.length);
	
	const aiAnimation* pAnimation = mScene->mAnimations[0];

	TYmat NodeTransformation = AssimpToGlm(pNode->mTransformation);
	
	const aiNodeAnim* pNodeAnim = nodeMapping[hashStr];

	if (pNodeAnim)
	{
		TYvec Scaling = GetPoseScale(pNodeAnim);
		TYmat ScalingM = glm::scale(TYmat(1.0f), { Scaling.x, Scaling.y, Scaling.z });

		TYquaternion RotationQ = GetPoseRotation(pNodeAnim);
		TYmat RotationM = glm::mat4_cast(RotationQ);

		TYvec Translation = GetPosePosition(pNodeAnim);
		TYmat TranslationM = glm::translate(TYmat(1.0f), { Translation.x, Translation.y, Translation.z });

		NodeTransformation = TranslationM * RotationM * ScalingM;
	}

	TYmat GlobalTransform = ParentTransform * NodeTransformation;

	auto it = boneMapping.find(hashStr);
	if (it != boneMapping.end())
	{
		TYuint BoneIndex = it->second;
		bones[BoneIndex].GlobalTransform = GlobalTransform;
		bones[BoneIndex].BoneTransform = globalInvMatrix * GlobalTransform * bones[BoneIndex].BoneOffset;
	}

	for (TYuint i = 0; i < pNode->mNumChildren; i++)
	{
		UpdatePose(pNode->mChildren[i], GlobalTransform);
	}
}

TYvec Animation::GetPosePosition(const aiNodeAnim* pNodeAnim)
{
	if (pNodeAnim->mNumPositionKeys == 1)
	{
		return AssimpToGlm(pNodeAnim->mPositionKeys[0].mValue);
	}

	TYuint it, curr = 0, step = 0;
	TYuint count = pNodeAnim->mNumPositionKeys - 1;

	while (count > 0)
	{
		it = curr;
		step = count / 2;

		it += step;

		if (((TYfloat)pNodeAnim->mPositionKeys[it].mTime < timeElapsed))
		{
			curr = ++it;
			count -= step + 1;
		}
		else
			count = step;
	}

	if (curr != 0)
		curr--;

	TYuint NextPositionIndex = (curr + 1);

	assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);

	TYfloat DeltaTime = (TYfloat)(pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[curr].mTime);
	TYfloat Factor = (timeElapsed - (TYfloat)pNodeAnim->mPositionKeys[curr].mTime) / DeltaTime;

	//assert(Factor >= 0.0f && Factor <= 1.0f);

	TYvec Start = AssimpToGlm(pNodeAnim->mPositionKeys[curr].mValue);
	TYvec End = AssimpToGlm(pNodeAnim->mPositionKeys[NextPositionIndex].mValue);
	TYvec Delta = End - Start;

	return Start + Factor * Delta;
}

TYvec Animation::GetPoseScale(const aiNodeAnim* pNodeAnim)
{
	if (pNodeAnim->mNumScalingKeys == 1)
	{
		return AssimpToGlm(pNodeAnim->mScalingKeys[0].mValue);
	}

	TYuint it, curr = 0, step = 0;
	TYuint count = pNodeAnim->mNumScalingKeys - 1;

	while (count > 0)
	{
		it = curr;
		step = count / 2;

		it += step;

		if (((TYfloat)pNodeAnim->mScalingKeys[it].mTime < timeElapsed))
		{
			curr = ++it;
			count -= step + 1;
		}
		else
			count = step;
	}

	if(curr != 0)
		curr--;

	TYuint NextScalingIndex = (curr + 1);
	assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
	TYfloat DeltaTime = (TYfloat)(pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[curr].mTime);
	TYfloat Factor = (timeElapsed - (TYfloat)pNodeAnim->mScalingKeys[curr].mTime) / DeltaTime;

	//assert(Factor >= 0.0f && Factor <= 1.0f);

	TYvec Start = AssimpToGlm(pNodeAnim->mScalingKeys[curr].mValue);
	TYvec End = AssimpToGlm(pNodeAnim->mScalingKeys[NextScalingIndex].mValue);
	TYvec Delta = End - Start;

	return Start + Factor * Delta;
}

TYquaternion Animation::GetPoseRotation(const aiNodeAnim* pNodeAnim)
{
	if (pNodeAnim->mNumRotationKeys == 1) 
	{
		return AssimpToGlm(pNodeAnim->mRotationKeys[0].mValue);
	}

	TYuint it, curr = 0, step = 0;
	TYuint count = pNodeAnim->mNumRotationKeys - 1;

	while (count > 0)
	{
		it = curr;
		step = count / 2;

		it += step;

		if (((TYfloat)pNodeAnim->mRotationKeys[it].mTime < timeElapsed))
		{
			curr = ++it;
			count -= step + 1;
		}
		else
			count = step;
	}

	if (curr != 0)
		curr--;

	TYuint NextRotationIndex = (curr + 1);

	assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);

	TYfloat DeltaTime = (TYfloat)(pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[curr].mTime);
	TYfloat Factor = (timeElapsed - (TYfloat)pNodeAnim->mRotationKeys[curr].mTime) / DeltaTime;

	//assert(Factor >= 0.0f && Factor <= 1.0f);
	TYquaternion StartRotationQ = AssimpToGlm(pNodeAnim->mRotationKeys[curr].mValue);
	TYquaternion EndRotationQ = AssimpToGlm(pNodeAnim->mRotationKeys[NextRotationIndex].mValue);

	//TYquaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
	aiQuaternion ooo;
	aiQuaternion::Interpolate(ooo, pNodeAnim->mRotationKeys[curr].mValue, pNodeAnim->mRotationKeys[NextRotationIndex].mValue, Factor);
	ooo.Normalize();

	return AssimpToGlm(ooo);
}

TYvoid Animation::DrawSkeleton(const TYmat& modelMatrix)
{
	for (TYuint i = 0; i < bones.size(); i++)
	{
		TYvec center = modelMatrix * bones[i].GlobalTransform * TYvec4(0.0f, 0.0f, 0.0f, 1.0f);
		GenericDraw::DrawSphere(center, 0.6f, TYvec(1.0f, 0.95f, 0.95f));
	}
}
