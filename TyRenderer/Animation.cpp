///////////////////////////////////////////////////////////////////////////////////////////////
// Handles animation updates
// Stores current animation, (optional) Animator 
///////////////////////////////////////////////////////////////////////////////////////////////

#include "Animation.h"

#include <SOIL2.h>
#include <deque>

#include "Transform.h"
#include "Animator.h"

#include "AnimationUtils.h"
#include "MeshUtils.h"
#include "AssimpUtils.h"

#include "Utils.h"

Animation::Animation()
{

}

Animation* Animation::CreateAnimation(const aiScene* scene, TYbool fbx)
{
	Animation* animation = new Animation();
	animation->fbx = fbx;

	aiAnimation* anim = scene->mAnimations[0];

	if (anim->mTicksPerSecond != 0.0f)
		animation->ticksPerSecond = (TYfloat)anim->mTicksPerSecond;
	else
		animation->ticksPerSecond = 1.0f;

	animation->duration = (TYfloat)anim->mDuration * (TYfloat)anim->mTicksPerSecond;
	animation->duration = (TYfloat)anim->mDuration;

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
;

	// Animation Time: duration * tickPerSecond / 60.0f

	timeElapsed = (timeElapsed >= duration ? dt * 0.2f : timeElapsed);
}

TYvoid Animation::UpdatePose()
{
	//for (TYpair<TYint, BoneKeyframes*> boneKeyframe : skeleton->skeleton)
	for (BoneKeyframes* keyframe : skeleton1)
	{
		//BoneKeyframes* keyframe = boneKeyframe.second;
		Bone* bone = keyframe->bone;

		TYmat parentMat = TYmat(1.0f);

		TYvec pt = TYvec();
		TYvec ps = TYvec(1);
		TYquaternion pr = TYvec();

		Bone* parent = bone->parent;

		if (parent != TYnull)
		{
			parentMat = parent->local;

			pt = parent->transform->Get<Transformation::Position>();
			ps = parent->transform->Get<Transformation::Scale>();
			pr = parent->transform->GetRotation();
		}
		/*while (parent != TYnull)
		{
			//parentMat = bone->parent->transform->GetMatrix();
			parentMat *= parent->transform->GetMatrix();

			parent = parent->parent;
		}*/

		TYvec position = GetPosePosition(keyframe->positionTimestamps, keyframe->positions);
		TYvec scale = GetPoseScale(keyframe->scaleTimestamps, keyframe->scales);
		TYquaternion rotation = GetPoseRotation(keyframe->rotationTimestamps, keyframe->rotations);

		bone->transform->Set<Transformation::Position>(position);
		bone->transform->Set<Transformation::Scale>(scale);
		bone->transform->Set(rotation);

		TYvec t = bone->transform->Get<Transformation::Position>();
		TYvec s = bone->transform->Get<Transformation::Scale>();
		TYquaternion r = bone->transform->GetRotation();

		TYmat mt = glm::translate(TYmat(1.0f), t);
		TYmat ms = glm::scale(TYmat(1.0f), s);
		TYmat mr = glm::toMat4(r);

		TYmat pmt = glm::translate(TYmat(1.0f), pt);
		TYmat pms = glm::scale(TYmat(1.0f), ps);
		TYmat pmr = glm::toMat4(pr);

		//TYmat Pls = ms * pms * mr * mt * pmr * pmt;
		//TYmat Pls = pmt * pmr * mt * mr * pms * ms;

		bone->local = parentMat * bone->transform->GetMatrix();

		if (keyframe->id != INT_MAX)
		{
			currPose[keyframe->id] = globalInvMatrix * bone->local * bone->offset;
		}
	}
}

TYvoid Animation::CreateSkeleton(const aiScene* scene)
{
	TYumap<TYint, TYmat> boneData;

	for (TYuint m = 0; m < scene->mNumMeshes; m++)
	{
		aiMesh* mesh = scene->mMeshes[m];

		for (TYuint i = 0; i < mesh->mNumBones; i++)
		{
			aiBone* bone = mesh->mBones[i];
			TYmat m = AssimpToGlm(bone->mOffsetMatrix);

			TYint hashStr = Hash(TYstring(bone->mName.C_Str()));
			if (boneData.find(hashStr) != boneData.end())
			{
				i = i;
			}
			boneData[hashStr] = m;
		}
	}

	currPose.resize(boneData.size(), TYmat(1.0f));
	skeleton = new Skeleton(scene->mRootNode, (TYint)boneData.size(), boneData);

	LoadKeyframes(skeleton->skeleton);
	//GenGlobalKeyframes();

	TYint boneLess = 0;

	for (TYpair<TYint, BoneKeyframes*> boneKeyframe : skeleton->skeleton)
	{
		skeleton1.push_back(boneKeyframe.second);
		if (boneKeyframe.second->id == INT_MAX) boneLess++;
	}

	std::sort(skeleton1.begin(), skeleton1.end(),
		[](const BoneKeyframes* a, const BoneKeyframes* b) -> bool
	{
		return a->id < b->id;
	});

	for (TYint i = 0; i < boneLess; i++)
	{
		BoneKeyframes* v = skeleton1[skeleton1.size() - 1];
		skeleton1.pop_back();
		skeleton1.insert(skeleton1.begin(), v);
	}
}

TYvoid Animation::LoadKeyframes(TYumap<TYint, BoneKeyframes*>& skeleton)
{
	const aiAnimation* anim = mScene->mAnimations[0];

	TYumap<TYstring, aiNodeAnim*> channels = {};
	TYumap<TYstring, TYvector<aiNodeAnim*>> channelsList = {};

	for (TYuint i = 0; i < anim->mNumChannels; i++)
	{
		aiNodeAnim* channel = anim->mChannels[i];

		TYstring str = channel->mNodeName.C_Str();
		channels[str] = channel;
	}

	for (TYuint i = 0; i < anim->mNumChannels; i++)
	{
		aiNodeAnim* channel = anim->mChannels[i];

		TYstring str = channel->mNodeName.C_Str();

		if (channels[str] != TYnull)
		{
			if (fbx)
			{
				TYsizet t = str.find_first_of("$");
				if (t != std::string::npos)
				{
					str = str.substr(0, t - 1);

					channelsList[str].push_back(channel);
				}
			}
			else
			{
				channelsList[str].push_back(channel);
			}
		}
	}

	for (TYpair<TYstring, TYvector<aiNodeAnim*>> channel : channelsList)
	{
		TYstring name = channel.first;
		TYint hashStr = Hash(name);

		if (skeleton.find(hashStr) == skeleton.end())
		{
			continue;
		}

		if (channel.second.size() == 1)
		{
			aiNodeAnim* aNode = channel.second[0];
			TYuint j = 0;
			for (j = 0; j < aNode->mNumPositionKeys; j++)
			{
				skeleton[hashStr]->positionTimestamps.push_back((TYfloat)aNode->mPositionKeys[j].mTime);
				skeleton[hashStr]->positions.push_back(AssimpToGlm(aNode->mPositionKeys[j].mValue));
			}
			skeleton[hashStr]->positionTimestamps.push_back((TYfloat)aNode->mPositionKeys[j - 1].mTime + 100.0f);
			skeleton[hashStr]->positions.push_back(AssimpToGlm(aNode->mPositionKeys[0].mValue));

			TYmat r = TYmat(1);
			TYvec pp = r * TYvec4(skeleton[hashStr]->positions[0], 1.0f);
			skeleton[hashStr]->bone->transform->Set<Transformation::Position>(pp);

			for (j = 0; j < aNode->mNumRotationKeys; j++)
			{
				skeleton[hashStr]->rotationTimestamps.push_back((TYfloat)aNode->mRotationKeys[j].mTime);
				skeleton[hashStr]->rotations.push_back(AssimpToGlm(aNode->mRotationKeys[j].mValue));
			}
			skeleton[hashStr]->rotationTimestamps.push_back((TYfloat)aNode->mRotationKeys[j - 1].mTime + 100.0f);
			skeleton[hashStr]->rotations.push_back(AssimpToGlm(aNode->mRotationKeys[0].mValue));

			for (j = 0; j < aNode->mNumScalingKeys; j++)
			{
				skeleton[hashStr]->scaleTimestamps.push_back((TYfloat)aNode->mScalingKeys[j].mTime);
				skeleton[hashStr]->scales.push_back(AssimpToGlm(aNode->mScalingKeys[j].mValue));
			}
			skeleton[hashStr]->scaleTimestamps.push_back((TYfloat)aNode->mScalingKeys[j - 1].mTime + 100.0f);
			skeleton[hashStr]->scales.push_back(AssimpToGlm(aNode->mScalingKeys[0].mValue));

			frameCount = TyMax({ frameCount, aNode->mNumPositionKeys, aNode->mNumRotationKeys, aNode->mNumScalingKeys });
		}
		else
		{
			for (aiNodeAnim* node : channel.second)
			{
				for (TYuint j = 0; j < node->mNumPositionKeys; j++)
				{
					aiVectorKey v = node->mPositionKeys[j];
					aiVector3D vv = v.mValue;
					vv.x += 0.0f;
				}

				TYstring cName = node->mNodeName.C_Str();

				if (cName.find("Translation") != TYstring::npos)
				{
					TYuint j = 0;
					for (; j < node->mNumPositionKeys; j++)
					{
						skeleton[hashStr]->positionTimestamps.push_back((TYfloat)node->mPositionKeys[j].mTime);
						skeleton[hashStr]->positions.push_back(AssimpToGlm(node->mPositionKeys[j].mValue));
					}
					skeleton[hashStr]->positionTimestamps.push_back((TYfloat)node->mPositionKeys[j - 1].mTime + 100.0f);
					skeleton[hashStr]->positions.push_back(AssimpToGlm(node->mPositionKeys[0].mValue));

					TYmat r = TYmat(1.0f);// glm::toMat4(TYquaternion(glm::radians(TYvec(-90, -90, 0))));
					//r = glm::translate(r, {});
					TYvec pp = r * TYvec4(skeleton[hashStr]->positions[0], 1.0f);
					skeleton[hashStr]->bone->transform->Set<Transformation::Position>(pp);
				}
				else if (cName.find("Rotation") != TYstring::npos)
				{
					TYuint j = 0;
					for (; j < node->mNumRotationKeys; j++)
					{
						skeleton[hashStr]->rotationTimestamps.push_back((TYfloat)node->mRotationKeys[j].mTime);
						skeleton[hashStr]->rotations.push_back(AssimpToGlm(node->mRotationKeys[j].mValue));
					}
					skeleton[hashStr]->rotationTimestamps.push_back((TYfloat)node->mRotationKeys[j - 1].mTime + 100.0f);
					skeleton[hashStr]->rotations.push_back(AssimpToGlm(node->mRotationKeys[0].mValue));
				}
				else if (cName.find("Scaling") != TYstring::npos)
				{
					TYuint j = 0;
					for (; j < node->mNumScalingKeys; j++)
					{
						skeleton[hashStr]->scaleTimestamps.push_back((TYfloat)node->mScalingKeys[j].mTime);
						skeleton[hashStr]->scales.push_back(AssimpToGlm(node->mScalingKeys[j].mValue));
					}
					skeleton[hashStr]->scaleTimestamps.push_back((TYfloat)node->mScalingKeys[j - 1].mTime + 100.0f);
					skeleton[hashStr]->scales.push_back(AssimpToGlm(node->mScalingKeys[0].mValue));
				}

				frameCount = TyMax({ frameCount, node->mNumPositionKeys, node->mNumRotationKeys, node->mNumScalingKeys });
			}
		}
	}
}

Skeleton::Skeleton(aiNode* root, TYint count, TYumap<TYint, TYmat>& boneData) : boneCount(count)
{
	name = root->mChildren[0]->mName.C_Str();

	TYint id = 0;
	GenSkeleton(root, TYnull, boneData, id);
}

TYvoid Skeleton::GenSkeleton(aiNode* node, Bone* parent, TYumap<TYint, TYmat>& boneData, TYint& id)
{
	TYstring str = node->mName.C_Str();
	TYint hashStr = Hash(str);

	Bone* newParent = parent;

	Bone* bone = TYnull;

	if (str.find("$") == TYstring::npos && node->mNumMeshes == 0)
	{
		TYuint ID = id;

		if (boneData.find(hashStr) != boneData.end())
		{
			bone = new Bone(str, parent, hashStr, boneData[hashStr]);
			id++;
		}
		else
		{
			bone = new Bone(str, parent, hashStr);
			ID = INT_MAX;
		}

		skeleton[hashStr] = new BoneKeyframes(bone, ID);

		if (parent)
		{
			///////////////////////////////////////////////////////////////////////////////////////
			//printf("\nNew Node: %s Parent: %s\n", node->mName.C_Str(), parent->name.c_str());
			//printf("***********************************************************************\n\n");
		}
		else
		{
			///////////////////////////////////////////////////////////////////////////////////////
			//printf("\nNew Node: %s Parent: NONE\n", node->mName.C_Str());
			//printf("***********************************************************************\n\n");
		}
	}

	for (TYuint i = 0; i < node->mNumChildren; i++)
	{
		if(bone) newParent = bone;
		GenSkeleton(node->mChildren[i], newParent, boneData, id);
	}
}

Bone::Bone(TYstring n, Bone* p, TYint keyId, TYmat off)
{
	name = n;
	parent = p;
	offset = off;
	keyframeId = keyId;

	transform = new Transform();
	transform->Set<Transformation::Rotation>(TYvec(-90, -90, 0));
}

TYvoid Animation::GenGlobalKeyframes()
{
	globalKeyframes = true;

	TYvector<BoneKeyframes*> Keyframes;
	TYint boneLess = 0;
	for (TYpair<TYint, BoneKeyframes*> boneKeyframe : skeleton->skeleton)
	{
		Keyframes.push_back(boneKeyframe.second);
		if (boneKeyframe.second->id == INT_MAX) boneLess++;
	}

	std::sort(Keyframes.begin(), Keyframes.end(),
		[](const BoneKeyframes* a, const BoneKeyframes* b) -> bool
	{
		return a->id < b->id;
	});

	//TYvector3 center = Keyframes[0]->positions;
	for (TYuint j = 0; j < Keyframes[0]->positions.size(); j++)
	{
		//Keyframes[0]->positions[j] = TYvec(0);
	}

	for (TYint i = 0; i < boneLess; i++)
	{
		BoneKeyframes* v = Keyframes[Keyframes.size() - 1];
		Keyframes.pop_back();
		Keyframes.insert(Keyframes.begin(), v);
	}

	for (BoneKeyframes* keyframe : Keyframes)
	{
		Bone* bone = keyframe->bone;
		if (bone->parent)
			printf("Me %s Parent %s\n", bone->name.c_str(), bone->parent->name.c_str());
		else
			printf("Me %s Parent NONE\n", bone->name.c_str());

		////////////////////////////////////////////////////////////////////////////////////
		BoneKeyframes* parentKeyframe = TYnull;
		if (bone->parent)
			parentKeyframe = skeleton->skeleton[bone->parent->keyframeId];

		if (parentKeyframe /*&& parentKeyframe->id != INT_MAX*/)
		{
			TYuint j = 0;
			for (j = 0; j < keyframe->positions.size(); j++)
			{
				TYvec myPos = keyframe->positions[j];
				TYvec parPos = TYvec(0);
				if(parentKeyframe->positions.size() > j)
					parPos = parentKeyframe->positions[j];

				myPos += parPos;

				keyframe->positions[j] = myPos;
				keyframe->bone->transform->Set<Transformation::Position>(keyframe->positions[0]);
			}
			if (keyframe->positions.size() == 0 && parentKeyframe->positions.size() > 0)
			{
				keyframe->positions.push_back(parentKeyframe->positions[0]);
				keyframe->positionTimestamps.push_back(duration);
				keyframe->bone->transform->Set<Transformation::Position>(keyframe->positions[0]);
			}
			for (j = 0; j < keyframe->rotations.size(); j++)
			{
				TYquaternion myRot = keyframe->rotations[j];
				TYquaternion parRot = TYquaternion(TYvec(0));
				if (parentKeyframe->rotations.size() > j)
					parRot = parentKeyframe->rotations[j];

				keyframe->rotations[j] = myRot * parRot;
				keyframe->bone->transform->Set(keyframe->rotations[0]);
			}
			for (j = 0; j < keyframe->scales.size(); j++)
			{
				TYvec myScale = keyframe->scales[j];
				TYvec parScale = TYvec(1);
				if (parentKeyframe->scales.size() > j)
					parScale = parentKeyframe->scales[j];

				keyframe->scales[j] = myScale * parScale;
				keyframe->bone->transform->Set<Transformation::Scale>(keyframe->scales[0]);
			}
		}
	}
	/*for (BoneKeyframes* keyframe : Keyframes)
	{
		for (TYuint j = 0; j < keyframe->positions.size(); j++)
		{
			TYvec myPos = keyframe->positions[j];
			TYmat r = TYmat(1.0f);
			r = glm::rotate(r, glm::radians(90.0f), TYvec(1, 0, 0));
			r = glm::rotate(r, glm::radians(90.0f), TYvec(0, 1, 0));
			r = glm::rotate(r, glm::radians(0.0f), TYvec(0, 0, 1));

			myPos = r * TYvec4(myPos, 1.0f);

			keyframe->positions[j] = myPos;
			keyframe->bone->transform->Set<Transformation::Position>(keyframe->positions[0]);
		}
	}*/
}

TYvec Animation::GetPosePosition(TYvectorF& timestamps, TYvector3& positions)
{
	if (timestamps.size() == 1)
	{
		return positions[0];
	}

	if (timestamps.size() == 0)
	{
		return TYvec();
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

	TYvec initPos = positions[segment - 1];
	TYvec nextPos = positions[segment];

	return glm::mix(initPos, nextPos, frac);
}

TYvec Animation::GetPoseScale(TYvectorF& timestamps, TYvector3& scales)
{
	if (timestamps.size() == 1)
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

	return glm::mix(initScale, nextScale, frac);
}

TYquaternion Animation::GetPoseRotation(TYvectorF& timestamps, TYvector<TYquaternion>& rotations)
{
	if (timestamps.size() == 1)
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

	return glm::slerp(initRot, nextRot, frac);
}
