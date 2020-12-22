///////////////////////////////////////////////////////////////////////////////////////////////
// Handles animation updates
// Stores current animation, (optional) Animator 
///////////////////////////////////////////////////////////////////////////////////////////////
//https://github.com/Robadob/sdl_exp/blob/master/sdl_exp/visualisation/model/Animation.h

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
	//skeleton1.clear();
	//delete skeleton;
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
	timeElapsed += dt * ticksPerSecond * 0.5f;

	// Animation Time: duration * tickPerSecond / 60.0f

	timeElapsed = (timeElapsed >= duration ? dt * 0.2f : timeElapsed);
}

TYvoid Animation::UpdatePose()
{
	TYmat Identity(1.0f);

	UpdatePose(mScene->mRootNode, Identity);

	for (TYuint i = 0; i < numBones; i++)
	{
		currentPose[i] = boneInfo[i].FinalTransformation;
	}
}

TYvoid Animation::UpdatePose(const aiNode* pNode, const TYmat& ParentTransform)
{
	//TYstring NodeName(pNode->mName.C_Str());
	TYint hashStr = Hash(pNode->mName.C_Str(), pNode->mName.length);
	
	const aiAnimation* pAnimation = mScene->mAnimations[0];

	TYmat NodeTransformation = AssimpToGlm(pNode->mTransformation);
	
	const aiNodeAnim* pNodeAnim = nodeMapping[hashStr];// GetNodeAnim(pAnimation, NodeName);//

	if (pNodeAnim)
	{
		/*TYvec Scaling;
		CalcInterpolatedScaling(Scaling, timeElapsed, pNodeAnim);
		TYmat ScalingM = glm::scale(TYmat(1.0f), { Scaling.x, Scaling.y, Scaling.z });

		TYquaternion RotationQ;
		CalcInterpolatedRotation(RotationQ, timeElapsed, pNodeAnim);
		TYmat RotationM = glm::mat4_cast(RotationQ);

		TYvec Translation;
		CalcInterpolatedPosition(Translation, timeElapsed, pNodeAnim);
		TYmat TranslationM = glm::translate(TYmat(1.0f), { Translation.x, Translation.y, Translation.z });*/

		/*TYvec position = GetPosePosition(pNodeAnim);
		TYvec scale = GetPoseScale(pNodeAnim);
		TYquaternion rotation = GetPoseRotation(pNodeAnim);

		TYmat ScalingM = glm::scale(TYmat(1.0f), scale);

		TYmat RotationM = glm::mat4_cast(rotation);

		TYmat TranslationM = glm::translate(TYmat(1.0f), position);*/

		TYvec Scaling;
		CalcInterpolatedScaling(Scaling, pNodeAnim);
		TYmat ScalingM = glm::scale(TYmat(1.0f), { Scaling.x, Scaling.y, Scaling.z });

		TYquaternion RotationQ;
		CalcInterpolatedRotation(RotationQ, pNodeAnim);
		TYmat RotationM = glm::mat4_cast(RotationQ);

		TYvec Translation;
		CalcInterpolatedPosition(Translation, pNodeAnim);
		TYmat TranslationM = glm::translate(TYmat(1.0f), { Translation.x, Translation.y, Translation.z });

		NodeTransformation = TranslationM * RotationM * ScalingM;
	}

	TYmat GlobalTransformation = ParentTransform * NodeTransformation;

	auto it = boneMapping.find(hashStr);
	if (it != boneMapping.end())
	{
		TYuint BoneIndex = it->second;
		boneInfo[BoneIndex].GlobalTransformation = GlobalTransformation;
		boneInfo[BoneIndex].FinalTransformation = globalInvMatrix * GlobalTransformation * boneInfo[BoneIndex].BoneOffset;
	}

	for (TYuint i = 0; i < pNode->mNumChildren; i++)
	{
		UpdatePose(pNode->mChildren[i], GlobalTransformation);
	}
}

const aiNodeAnim* Animation::GetNodeAnim(const aiAnimation* pAnimation, TYstring& NodeName)
{
	static TYumap<TYstring, aiNodeAnim*> animMap;

	aiNodeAnim* anim = TYnull;

	if (animMap.size() > 0)
	{
		anim = animMap[NodeName];
	}
	else
	{
		for (TYuint i = 0; i < pAnimation->mNumChannels; i++) 
		{
			const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];

			animMap.insert({ TYstring(pNodeAnim->mNodeName.data), (aiNodeAnim*)pNodeAnim });

			if (TYstring(pNodeAnim->mNodeName.data) == NodeName)
			{
				anim = (aiNodeAnim*)pNodeAnim;
			}
		}
	}


	return anim;
}

TYvec Animation::GetPosePosition(const aiNodeAnim* pNodeAnim)
{
	/*if (timestamps.size() == 1)
	{
		return positions[0];
	}

	if (timestamps.size() == 0)
	{
		return TYvec();
	}*/
	if (pNodeAnim->mNumPositionKeys == 1)
	{
		return AssimpToGlm(pNodeAnim->mPositionKeys[0].mValue);
	}

	TYuint curr = 0;
	/*while (timeElapsed > timestamps[segment])
	{
		segment++;
	}

	if (segment == 0) segment++;*/

	for (TYuint i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++)
	{
		if (timeElapsed < (TYfloat)pNodeAnim->mPositionKeys[i + 1].mTime)
		{
			curr = i;
			break;
		}
	}

	TYuint next = curr + 1;

	assert(next < pNodeAnim->mNumPositionKeys);

	TYfloat DeltaTime = (TYfloat)(pNodeAnim->mPositionKeys[next].mTime - pNodeAnim->mPositionKeys[curr].mTime);
	TYfloat Factor = (timeElapsed - (TYfloat)pNodeAnim->mPositionKeys[curr].mTime) / DeltaTime;

	//assert(Factor >= 0.0f && Factor <= 1.0f);

	TYvec startPos = AssimpToGlm(pNodeAnim->mPositionKeys[curr].mValue);
	TYvec endPos = AssimpToGlm(pNodeAnim->mPositionKeys[next].mValue);

	TYvec Delta = endPos - startPos;

	return startPos + Factor * Delta;

	//return glm::mix(initPos, nextPos, frac);
}

TYvec Animation::GetPoseScale(const aiNodeAnim* pNodeAnim)
{
	/*if (timestamps.size() == 1)
	{
		return scales[0];
	}

	if (timestamps.size() == 0)
	{
		return TYvec(1.0f);
	}

	TYuint segment = 0;
	while (timeElapsed > timestamps[segment])
	{
		segment++;
	}

	if (segment == 0) segment++;

	TYfloat start = timestamps[segment - 1];
	TYfloat end = timestamps[segment];
	TYfloat frac = (timeElapsed - start) / (end - start);

	TYvec initScale = scales[segment - 1];
	TYvec nextScale = scales[segment];

	return glm::mix(initScale, nextScale, frac);*/

	if (pNodeAnim->mNumScalingKeys == 1)
	{
		return AssimpToGlm(pNodeAnim->mScalingKeys[0].mValue);
	}

	TYuint curr = 0;
	for (TYuint i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++) 
	{
		if (timeElapsed < (TYfloat)pNodeAnim->mScalingKeys[i + 1].mTime) 
		{
			curr = i;
			break;
		}
	}

	TYuint next = curr + 1;

	assert(next < pNodeAnim->mNumScalingKeys);

	TYfloat DeltaTime = (TYfloat)(pNodeAnim->mScalingKeys[next].mTime - pNodeAnim->mScalingKeys[curr].mTime);
	TYfloat Factor = (timeElapsed - (TYfloat)pNodeAnim->mScalingKeys[curr].mTime) / DeltaTime;

	//assert(Factor >= 0.0f && Factor <= 1.0f);

	TYvec startScale = AssimpToGlm(pNodeAnim->mScalingKeys[curr].mValue);
	TYvec endScale = AssimpToGlm(pNodeAnim->mScalingKeys[next].mValue);

	TYvec Delta = endScale - startScale;

	return startScale + Factor * Delta;
}

TYquaternion Animation::GetPoseRotation(const aiNodeAnim* pNodeAnim)
{
	/*if (timestamps.size() == 1)
	{
		return rotations[0];
	}

	if (timestamps.size() == 0)
	{
		return TYquaternion(TYvec4(0.0f, 0.0f, 0.0f, 1.0f));
	}

	TYuint segment = 0;
	while (timeElapsed > timestamps[segment])
	{
		segment++;
	}

	if (segment == 0) segment++;

	TYfloat start = timestamps[segment - 1];
	TYfloat end = timestamps[segment];
	TYfloat frac = (timeElapsed - start) / (end - start);

	TYquaternion initRot = rotations[segment - 1];
	TYquaternion nextRot = rotations[segment];

	return glm::slerp(initRot, nextRot, frac);*/

	if (pNodeAnim->mNumRotationKeys == 1) 
	{
		return AssimpToGlm(pNodeAnim->mRotationKeys[0].mValue);
	}

	TYuint curr = 0;

	for (TYuint i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++)
	{
		if (timeElapsed < (TYfloat)pNodeAnim->mRotationKeys[i + 1].mTime) 
		{
			curr = i;
		}
	}

	TYuint next = curr + 1;

	assert(next < pNodeAnim->mNumRotationKeys);

	TYfloat DeltaTime = (TYfloat)(pNodeAnim->mRotationKeys[next].mTime - pNodeAnim->mRotationKeys[curr].mTime);
	TYfloat Factor = (timeElapsed - (TYfloat)pNodeAnim->mRotationKeys[curr].mTime) / DeltaTime;

	//assert(Factor >= 0.0f && Factor <= 1.0f);

	aiQuaternion rot;
	aiQuaternion::Interpolate(rot, pNodeAnim->mRotationKeys[curr].mValue, pNodeAnim->mRotationKeys[next].mValue, Factor);
	rot.Normalize();

	return AssimpToGlm(rot);
}

TYvoid Animation::DrawSkeleton()
{
	for (TYuint i = 0; i < boneInfo.size(); i++)
	{
		{
			TYvec center = boneInfo[i].GlobalTransformation * TYvec4(0.0f, 0.0f, 0.0f, 1.0f);
			//GenericDraw::DrawLine(parentCenter, center, TYvec(0.0f), 1.5f);
			GenericDraw::DrawSphere(center, 0.6f, TYvec(1.0f, 0.95f, 0.95f));
		}

		{
			TYvec center = boneInfo[i].FinalTransformation * TYvec4(0.0f, 0.0f, 0.0f, 1.0f);
			GenericDraw::DrawSphere(center, 0.3f, TYvec(0.85f, 0.15f, 0.15f));
		}
	}
}

TYuint Animation::FindPosition(const aiNodeAnim* pNodeAnim)
{
	for (TYuint i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++)
	{
		if (timeElapsed < (float)pNodeAnim->mPositionKeys[i + 1].mTime)
		{
			return i;
		}
	}

	//assert(0);

	return 0;
}

TYuint Animation::FindRotation(const aiNodeAnim* pNodeAnim)
{
	assert(pNodeAnim->mNumRotationKeys > 0);

	for (TYuint i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++) {
		if (timeElapsed < (float)pNodeAnim->mRotationKeys[i + 1].mTime) {
			return i;
		}
	}

	//assert(0);

	return 0;
}

TYuint Animation::FindScaling(const aiNodeAnim* pNodeAnim)
{
	assert(pNodeAnim->mNumScalingKeys > 0);

	for (TYuint i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++) 
	{
		if (timeElapsed < (float)pNodeAnim->mScalingKeys[i + 1].mTime) 
		{
			return i;
		}
	}

	//assert(0);

	return 0;
}

TYvoid Animation::CalcInterpolatedPosition(TYvec& Out, const aiNodeAnim* pNodeAnim)
{
	if (pNodeAnim->mNumPositionKeys == 1)
	{
		Out = AssimpToGlm(pNodeAnim->mPositionKeys[0].mValue);
		return;
	}

	TYuint PositionIndex = FindPosition(pNodeAnim);
	TYuint NextPositionIndex = (PositionIndex + 1);
	assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
	float DeltaTime = (float)(pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime);
	float Factor = (timeElapsed - (float)pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
	//assert(Factor >= 0.0f && Factor <= 1.0f);
	const TYvec& Start = AssimpToGlm(pNodeAnim->mPositionKeys[PositionIndex].mValue);
	const TYvec& End = AssimpToGlm(pNodeAnim->mPositionKeys[NextPositionIndex].mValue);
	TYvec Delta = End - Start;
	Out = Start + Factor * Delta;
}

TYvoid Animation::CalcInterpolatedRotation(TYquaternion& Out, const aiNodeAnim* pNodeAnim)
{
	// we need at least two values to interpolate...
	if (pNodeAnim->mNumRotationKeys == 1) {
		Out = AssimpToGlm(pNodeAnim->mRotationKeys[0].mValue);
		return;
	}

	TYuint RotationIndex = FindRotation(pNodeAnim);
	TYuint NextRotationIndex = (RotationIndex + 1);
	assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
	float DeltaTime = (float)(pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime);
	float Factor = (timeElapsed - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
	//assert(Factor >= 0.0f && Factor <= 1.0f);
	const TYquaternion& StartRotationQ = AssimpToGlm(pNodeAnim->mRotationKeys[RotationIndex].mValue);
	const TYquaternion& EndRotationQ = AssimpToGlm(pNodeAnim->mRotationKeys[NextRotationIndex].mValue);
	//TYquaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
	aiQuaternion ooo;
	aiQuaternion::Interpolate(ooo, pNodeAnim->mRotationKeys[RotationIndex].mValue, pNodeAnim->mRotationKeys[NextRotationIndex].mValue, Factor);
	ooo.Normalize();

	Out = AssimpToGlm(ooo);
}

TYvoid Animation::CalcInterpolatedScaling(TYvec& Out, const aiNodeAnim* pNodeAnim)
{
	if (pNodeAnim->mNumScalingKeys == 1)
	{
		Out = AssimpToGlm(pNodeAnim->mScalingKeys[0].mValue);
		return;
	}

	TYuint ScalingIndex = FindScaling(pNodeAnim);
	TYuint NextScalingIndex = (ScalingIndex + 1);
	assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
	float DeltaTime = (float)(pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime);
	float Factor = (timeElapsed - (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
	//assert(Factor >= 0.0f && Factor <= 1.0f);
	const TYvec& Start = AssimpToGlm(pNodeAnim->mScalingKeys[ScalingIndex].mValue);
	const TYvec& End = AssimpToGlm(pNodeAnim->mScalingKeys[NextScalingIndex].mValue);
	TYvec Delta = End - Start;
	Out = Start + Factor * Delta;
}
