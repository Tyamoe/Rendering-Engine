#ifndef TYAMOE3D

#include "stdafx.h"
#include "Utils.h"
#include "Globals.h"

#else

#include "Tyamoe3DHelper.h"
#include EngineInc(stdafx.h)
#include EngineInc(Utils.h)
#include EngineInc(Globals.h)

#endif // TYAMOE3D

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "RendererDeferred.h"
#include "Input.h"
#include "Entity.h"
#include "Material.h"
#include "Animation.h"
#include "AnimationUtils.h"
#include "Transform.h"

static TYvector<Light> Lights;

static TYbool tempPlay = false;

static TYuint albedoMap = 0;
static TYuint normalMap = 0;
static TYuint metallicMap = 0;
static TYuint roughnessMap = 0;
static TYuint aoMap = 0;

static TYvec tmpVec = {0, 0, 0};

TYvoid DrawSkeleton(TYumap<TYint, BoneKeyframes*>& skeleton)
{
	//glDisable(GL_DEPTH_TEST);
	for (TYpair<TYint, BoneKeyframes*> boneKeyframe : skeleton)
	{
		BoneKeyframes* keyframe = boneKeyframe.second;
		Bone* bone = keyframe->bone;

		TYvec parentPosition = TYvec();
		if (bone->parent != TYnull)
		{
			//parentPosition = bone->parent->transform->Get<Transformation::Position>();
		}

		TYvec position = bone->transform->Get<Transformation::Position>() + parentPosition;

		//TYmat r = glm::toMat4(TYquaternion(glm::radians(tmpVec)));
		TYmat r = TYmat(1.0f);
		r = glm::rotate(r, glm::radians(90.0f), TYvec(1, 0, 0));
		r = glm::rotate(r, glm::radians(90.0f), TYvec(0, 1, 0));
		r = glm::rotate(r, glm::radians(0.0f), TYvec(0, 0, 1));

		//r = glm::translate(r, bone->transform->Get<Transformation::Position>());
		//r = glm::translate(r, parentPosition);

		TYvec pp = bone->local * TYvec4(0, 0, 0, 1.0f);
		//pp += parentPosition;

		//pp += dpp;
		//skeleton2->skeleton[hashStr]->bone->transform->Set<Transformation::Position>(pp);

		if(!tempPlay)
			pp = bone->transform->Get<Transformation::Position>();

		TYvec dpp = r * TYvec4(pp, 1.0f);

		//pp = dpp;

		bone->globalPosition = position;

		if (keyframe->id != INT_MAX)
		{
			TYvec color = {};
			{
				if (keyframe->id == 0)
				{
					color = { 1, 0, 0 };
				}
				else if (keyframe->id <= 4)
				{
					color = { 0, 0.5f, 1 };
				}
				else if (keyframe->id <= 8)
				{
					color = { 0, 0, 1 };
				}
				else if (keyframe->id <= 10)
				{
					color = { 0, 1, 0 };
				}
				else if (keyframe->id == 19)
				{
					color = { 1, 1, 1 };
				}
				else if (keyframe->id == 20)
				{
					color = { 1, 1, 1 };
				}
				else if (keyframe->id >= 15)
				{
					color = { 0.6f, 0.6f, 0 };
				}
				else if (keyframe->id >= 11)
				{
					color = { 1, 1, 0 };
				}
			}
			GenericDraw::DrawLine(parentPosition, pp, TYvec(0), 1.0f);
			GenericDraw::DrawSphere(pp, 0.4f, color);
		}
	}
	//glEnable(GL_DEPTH_TEST);
}

TYvoid RenderDeferred::Draw(Entity* entity)
{
	MeshPtr mesh = entity->Get<MeshPtr>();
	Animation* anim = entity->Get<Animation*>();
	Material* mat = mesh->Get<Material*>();

	if (mat->mShader == MaterialShader::Phong)
	{
		ShaderPtr shader = BufferShader;

		if (mesh->IsAnimated())
		{
			shader = animBufferShader;
		}

		shader->Use();

		TYmat modelMatrix = entity->GetMatrix();
		TYmat modelInv = glm::inverse(modelMatrix);
		modelInv = glm::transpose(modelInv);
		TYmat MVP = GenericDraw::projection * camera->view * modelMatrix;

		shader->Uniforms["InvTrModel"](modelInv);
		shader->Uniforms["MVP"](MVP);
		shader->Uniforms["Model"](modelMatrix);

		for (TYsizet i = 0; i < mesh->geoCount(); i++)
		{
			if (mesh->IsAnimated())
			{
				//TYvector<TYmat> Transforms;
				//anim->BoneTransform((TYint)i, Transforms);
				//shader->Uniforms["SkeletonTransform"](Transforms, (TYint)Transforms.size());
				shader->Uniforms["SkeletonTransform"](anim->currPose, (TYint)anim->currPose.size());

				//shader->Uniforms["SkeletonTransform"](anim->currentPoseList[i], (TYint)anim->currentPoseList[i].size());
			}

			TYvec4 c = TYvec4(0.54f);
			c[i % 3] = 0.0f;
			c.a = 1.0f;
			shader->Uniforms["meshColor"](mat->color);

			MeshHandle mHandle = mesh->GetHandle((TYint)i);

			glBindVertexArray(mHandle.VAO);
			glDrawElements(GL_TRIANGLES, (TYint)mHandle.indexCount, GL_UNSIGNED_INT, 0);
		}
	}
	else
	{
		Draw(entity, true);
	}
}

TYvoid RenderDeferred::Draw(Entity* entity, TYbool)
{
	MeshPtr mesh = entity->Get<MeshPtr>();
	Animation* anim = entity->Get<Animation*>();
	Material* mat = mesh->Get<Material*>();

	//if (mat->mShader == MaterialShader::PBR)
	{
		PBRCount++;

		BufferPBRShader->Use();

		TYmat modelMatrix = entity->GetMatrix();
		TYmat MVP = GenericDraw::projection * camera->view * modelMatrix;

		BufferPBRShader->Uniforms["MVP"](MVP);
		BufferPBRShader->Uniforms["Model"](modelMatrix);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, albedoMap);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, normalMap);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, metallicMap);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, roughnessMap);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, aoMap);

		for (TYsizet i = 0; i < mesh->geoCount(); i++)
		{
			MeshHandle mHandle = mesh->GetHandle((TYint)i);

			glBindVertexArray(mHandle.VAO);
			glDrawElements(GL_TRIANGLES, (TYint)mHandle.indexCount, GL_UNSIGNED_INT, 0);
		}

		/*glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, albedoMap);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, normalMap);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, metallicMap);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, roughnessMap);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, aoMap);

		BufferPBRShader->Uniforms["albedoMap"](0);
		BufferPBRShader->Uniforms["normalMap"](1);
		BufferPBRShader->Uniforms["metallicMap"](2);
		BufferPBRShader->Uniforms["roughnessMap"](3);
		BufferPBRShader->Uniforms["aoMap"](4);

		PhongShader->Uniforms["lightCount"]((TYint)Lights.size());
		PhongShader->Uniforms["viewPos"](camera->position);
		PhongShader->Uniforms["ambient"]({ 0.96f, 0.96f, 0.96f });
		PhongShader->Uniforms["fog"]({ 0.7f, 0.7f, 0.7f });
		PhongShader->Uniforms["ZNear"](0.1f);
		PhongShader->Uniforms["ZFar"](300.0f);

		for (TYsizet i = 0; i < Lights.size(); i++)
		{
			TYstring lname = "lights[" + std::to_string(i) + "].";
			PhongShader->Uniforms[(TYchar*)(lname + "type").c_str()](Lights[i].type);
			PhongShader->Uniforms[(TYchar*)(lname + "ambi").c_str()](Lights[i].ambi);
			PhongShader->Uniforms[(TYchar*)(lname + "diff").c_str()](Lights[i].diff);
			PhongShader->Uniforms[(TYchar*)(lname + "spec").c_str()](Lights[i].spec);

			PhongShader->Uniforms[(TYchar*)(lname + "pos").c_str()](Lights[i].pos);
			PhongShader->Uniforms[(TYchar*)(lname + "c0").c_str()](Lights[i].c0);
			PhongShader->Uniforms[(TYchar*)(lname + "c1").c_str()](Lights[i].c1);
			PhongShader->Uniforms[(TYchar*)(lname + "c2").c_str()](Lights[i].c2);

			PhongShader->Uniforms[(TYchar*)(lname + "dir").c_str()](Lights[i].dir);

			PhongShader->Uniforms[(TYchar*)(lname + "innerTheta").c_str()](Lights[i].innerTheta);
			PhongShader->Uniforms[(TYchar*)(lname + "outerTheta").c_str()](Lights[i].outerTheta);
			PhongShader->Uniforms[(TYchar*)(lname + "spotIntensity").c_str()](Lights[i].spotIntensity);
		}

		for (TYsizet i = 0; i < mesh->geoCount(); i++)
		{
			MeshHandle mHandle = mesh->GetHandle((TYint)i);

			glBindVertexArray(mHandle.VAO);
			glDrawElements(GL_TRIANGLES, (TYint)mHandle.indexCount, GL_UNSIGNED_INT, 0);
		}*/
	}
}

TYvoid RenderDeferred::LightPass()
{
	Shader* shader[] =
	{
		PhongShader,
		PBRShader,
	};

	glDisable(GL_DEPTH_TEST);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (TYint s = 0; s < 2; s++)
	{
		if (s == 1 && PBRCount == 0) continue;
		shader[s]->Use();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gPosition);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gNormal);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, gAlbedo);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, gExtra);

		shader[s]->Uniforms["positionTexture"](0);
		shader[s]->Uniforms["normalTexture"](1);
		shader[s]->Uniforms["colorTexture"](2);
		shader[s]->Uniforms["extraTexture"](3);
			   
		shader[s]->Uniforms["lightCount"]((TYint)Lights.size());
		shader[s]->Uniforms["viewPos"](camera->position);
		shader[s]->Uniforms["ambient"]({ 0.96f, 0.96f, 0.96f });
		shader[s]->Uniforms["fog"]({ 0.7f, 0.7f, 0.7f });
		shader[s]->Uniforms["ZNear"](0.1f);
		shader[s]->Uniforms["ZFar"](300.0f);

		for (TYsizet i = 0; i < Lights.size(); i++)
		{
			TYstring lname = "lights[" + std::to_string(i) + "].";
			shader[s]->Uniforms[(TYchar*)(lname + "type").c_str()](Lights[i].type);
			shader[s]->Uniforms[(TYchar*)(lname + "ambi").c_str()](Lights[i].ambi);
			shader[s]->Uniforms[(TYchar*)(lname + "diff").c_str()](Lights[i].diff);
			shader[s]->Uniforms[(TYchar*)(lname + "spec").c_str()](Lights[i].spec);
				   
			shader[s]->Uniforms[(TYchar*)(lname + "pos").c_str()](Lights[i].pos);
			shader[s]->Uniforms[(TYchar*)(lname + "c0").c_str()](Lights[i].c0);
			shader[s]->Uniforms[(TYchar*)(lname + "c1").c_str()](Lights[i].c1);
			shader[s]->Uniforms[(TYchar*)(lname + "c2").c_str()](Lights[i].c2);
				   
			shader[s]->Uniforms[(TYchar*)(lname + "dir").c_str()](Lights[i].dir);
				   
			shader[s]->Uniforms[(TYchar*)(lname + "innerTheta").c_str()](Lights[i].innerTheta);
			shader[s]->Uniforms[(TYchar*)(lname + "outerTheta").c_str()](Lights[i].outerTheta);
			shader[s]->Uniforms[(TYchar*)(lname + "spotIntensity").c_str()](Lights[i].spotIntensity);
		}

		shader[s]->DrawQuad();
	}

	glEnable(GL_DEPTH_TEST);
}

TYvoid RenderDeferred::Deferred()
{
	Layout layout = window->GetLayout();

	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);

	//glDisable(GL_BLEND);

	for (TYuint i = 0; i < scene->entityList.size(); i++)
	{
		Draw(scene->entityList[i]);
	}

	//glEnable(GL_BLEND);

	glBindFramebuffer(GL_FRAMEBUFFER, RenderBuffer);
	glViewport(0, 0, layout.width, layout.height);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	LightPass();
}

TYvoid RenderDeferred::Forward()
{
	// UI
	{
		ImGui::Begin("Details");

		ImGui::Text("Camera Pos: %.3f, %.3f, %.3f", camera->position.x, camera->position.y, camera->position.z);
		ImGui::Text("Camera Front: %.3f, %.3f, %.3f", camera->front.x, camera->front.y, camera->front.z);

		ImGui::NewLine();

		TransformPtr t = Entity::Get<TransformPtr>("Test");

		TYvec p = t->Get<Transformation::Position>();
		TYvec s = t->Get<Transformation::Scale>();

		ImGui::DragFloat3("Light Pos", &p.x, 2.0f);
		ImGui::DragFloat3("Light Scale", &s.x, 0.5f);

		t->Set<Transformation::Position>(p);
		t->Set<Transformation::Scale>(s);

		Lights[1].pos = p;

		ImGui::NewLine();

		if (ImGui::Button("Compile Shaders"))
		{
			CompileShaders();
		}

		ImGui::End();
	}

	for (TYuint i = 0; i < scene->geometry.size(); i++)
	{
		if (scene->geometry[i]->GetType() == geoModel)
		{
			if (((Model*)scene->geometry[i])->octree)
			{
				((Model*)scene->geometry[i])->octree->Draw(((Model*)scene->geometry[i])->octree->root);
			}
		}
	}

	for (TYuint i = 0; i < scene->entityList.size(); i++)
	{
		Animation* anim = scene->entityList[i]->Get<Animation*>();
		if (anim)
		{
			DrawSkeleton(anim->skeleton->skeleton);
		}
	}
}

TYvoid RenderDeferred::PreRender()
{

}

static TYvector3 lyons;
static TYfloat frameTime = 0.0f;
static TYint frameNum = 0;

TYvoid RenderDeferred::Render(TYfloat dt)
{
	Layout layout = window->GetLayout();
	TYint width = layout.width;
	TYint height = layout.height;

	camera->Update(dt);

	GenericDraw::dt = dt;
	GenericDraw::projection = glm::perspective(glm::radians(Global::FOV), TYfloat(width) / height, 0.5f, 2000.0f);;
	GenericDraw::view = camera->view;
	
	for (TYuint i = 0; i < scene->entityList.size(); i++)
	{
		Animation* anim = scene->entityList[i]->Get<Animation*>();
		if (anim)
		{
			//UI
			{
				ImGui::Begin("Animations");

				ImGui::Checkbox("Play Animation", &tempPlay);

				ImGui::NewLine();

				ImGui::Text("Duration: %.3f", anim->duration);
				ImGui::Text("Elapsed: %.3f", anim->timeElapsed);

				ImGui::InputFloat("Frame Time", &frameTime);

				if (ImGui::Button("Skip To Frame Time"))
				{
					anim->SetFrame(frameTime);
				}

				ImGui::SliderInt("Frame", &frameNum, 0, anim->frameCount);

				if (ImGui::Button("Skip To Frame"))
				{
					anim->SetFrame(frameNum);
				}

				ImGui::End();
			}

			//UI2
			{
				ImGui::Begin("Object");

				ImGui::SliderFloat3("rr", &tmpVec.x, -360.0f, 360.0f);

				ImGui::NewLine();

				Transform* tr = scene->entityList[i]->Get<Transform*>();

				TYquaternion rot = tr->GetRotation();

				TYvec r = tr->Get<Transformation::Rotation>(); // { rot.x, rot.y, rot.z};

				TYvec sss = r;

				ImGui::SliderFloat3("R", &r.x, -360.0f, 360.0f);

				if (r != sss)
				{
					//rot = TYquaternion(r);
					tr->Set<Transformation::Rotation>(r);
				}

				ImGui::End();
			}

			if (tempPlay)
			{
				anim->Update(dt);
				anim->UpdatePose();
				//for (TYuint m = 0; m < anim->currentPoseList.size(); m++)
				//	anim->UpdatePose(anim->skeletonList[m], TYmat(1.0f), anim->globalInvMatrix, m);
			}
		}
	}

	Deferred();

	glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, RenderBuffer);
	glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, RenderBuffer);

	//////////////////////////////////////////////////////////////////////////

	if (TY::in->isMouseReleased(MouseRight))
	{
		for (TYuint i = 0; i < scene->geometry.size(); i++)
		{
			if (scene->geometry[i]->GetType() == geoModel)
			{
				if (((Model*)scene->geometry[i])->octree)
				{
					((Model*)scene->geometry[i])->octree->Traverse(((Model*)scene->geometry[i])->octree->root);
				}
			}
		}
	}
	if (TY::in->isMouseReleased(MouseMiddle))
	{
		TYfloat y_, x_;

		x_ = width / 2.0f; //TY::in->mouse.screenPos.x;
		y_ = height / 2.0f; //TY::in->mouse.screenPos.y;

		TYvec2 screenCoord;
		screenCoord.x = (2.0f * x_) / width - 1.0f;
		screenCoord.y = (-2.0f * (height - y_)) / height + 1.0f;

		TYvec dir = camera->front + screenCoord.x * camera->dim.x * camera->right +
			screenCoord.y * camera->dim.y * camera->up;

		TYvec rayOrigin, rayDir;

		rayDir = glm::normalize(dir);
		rayOrigin = camera->position;

		TYvec normal = TYvec(0.0f);
		TYfloat tnear = TYinf;
		Geometry* hit = TYnull;
		TYint hitIndex = -1;

		for (TYuint i = 0; i < scene->geometry.size(); i++)
		{
			TYvec norm = TYvec(0.0f);
			TYfloat t0 = TYinf, t1 = TYinf;

			if (scene->geometry[i]->Intersect(rayOrigin, rayDir, t0, t1, norm))
			{
				if (t0 < 0)
				{
					t0 = t1;
				}

				if (t0 < tnear)
				{
					tnear = t0;
					hit = scene->geometry[i];
					normal = norm;
					hitIndex = i;


				}
			}
		}

		// No intersection
		if (!hit)
		{
			//GenericDraw::DrawLine(camera->position, camera->position + rayDir * 41.0f, TYvec(0), 2.0f);
			lyons.push_back(camera->position);
			lyons.push_back(camera->position + rayDir * 41.0f);
			//TYlog << "No hit\n";
		}
		else
		{
			//GenericDraw::DrawLine(camera->position, hit->center, TYvec(0), 2.0f);
			lyons.push_back(camera->position);
			lyons.push_back(camera->position + rayDir * tnear);
			//lyons.push_back(hit->center);
		}
	}

	for (TYsizet i = 0; i < lyons.size(); i+=2)
	{
		GenericDraw::DrawLine(lyons[i], lyons[i+1], TYvec(0), 2.0f);
	}

	Lights[0].pos = camera->position;

	//////////////////////////////////////////////////////////////////////////

	Forward();
}

TYvoid RenderDeferred::PostRender()
{
	QuadShader->Use();
	QuadShader->DrawQuad(RenderTexture);

	PBRCount = 0;
}

TYvoid RenderDeferred::CompileShaders()
{
	animBufferShader = new Shader("deferredBuffer_Anim.vs", "deferredBuffer.fs");
	BufferShader = new Shader("deferredBuffer.vs", "deferredBuffer.fs");
	PhongShader = new Shader("deferredPhong.vs", "deferredPhong.fs");

	BufferPBRShader = new Shader("deferredBufferPBR.vs", "deferredBufferPBR.fs");
	PBRShader = new Shader("deferredPBR.vs", "deferredPBR.fs");

	QuadShader = new Shader("quad.vs", "quad.fs");
}

TYvoid RenderDeferred::Init()
{
	Layout layout = window->GetLayout();

	TYint width = layout.width;
	TYint height = layout.height;

	camera = new Camera(TYnull, false);

	// Setup frameBuffer
	glGenFramebuffers(1, &RenderBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, RenderBuffer);

	// Setup render texture
	glGenTextures(1, &RenderTexture);
	glBindTexture(GL_TEXTURE_2D, RenderTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, RenderTexture, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer in Deferred Renderer is not complete!" << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Deferred
	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

	glGenTextures(1, &gPosition);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

	glGenTextures(1, &gAlbedo);
	glBindTexture(GL_TEXTURE_2D, gAlbedo);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedo, 0);

	glGenTextures(1, &gExtra);
	glBindTexture(GL_TEXTURE_2D, gExtra);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32UI, width, height, 0, GL_RGBA_INTEGER, GL_UNSIGNED_INT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, gExtra, 0);

	glGenTextures(1, &gDepth);
	glBindTexture(GL_TEXTURE_2D, gDepth);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, gDepth, 0);

	unsigned int attachments[4] = 
	{ 
		GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 
	};
	glDrawBuffers(4, attachments);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

RenderDeferred::RenderDeferred() : Renderer()
{
	SetType(RendererType::Deferred);

	animBufferShader = new Shader("deferredBuffer_Anim.vs", "deferredBuffer.fs");
	BufferShader = new Shader("deferredBuffer.vs", "deferredBuffer.fs");
	PhongShader = new Shader("deferredPhong.vs", "deferredPhong.fs");

	BufferPBRShader = new Shader("deferredBufferPBR.vs", "deferredBufferPBR.fs");
	PBRShader = new Shader("deferredPBR.vs", "deferredPBR.fs");

	QuadShader = new Shader("quad.vs", "quad.fs");

	scene = new Scene();
	scene->GenHandles_GL();
	scene->GenOctree();

	Light light;
	light.type = L_POINT;

	light.ambi = { 0.8f, 0.0f, 0.0f };
	light.diff = { 0.95f, 0.05f, 0.0f };
	light.spec = { 0.77f, 0.0f, 0.0f };

	light.pos = { 0.0f, 30.0f, 0.0f };

	light.c0 = 1.0f;
	light.c1 = 0.05f;
	light.c2 = 0.0075f;

	light.dir = { 0.1f, -1.0f, -0.11f };

	light.innerTheta = 11.0f;
	light.outerTheta = 20.0f;
	light.spotIntensity = 5.0f;

	Lights.push_back(light);

	light.ambi = { 0.61f, 0.61f, 0.0f };
	light.diff = { 0.85f, 0.85f, 0.1f };
	light.spec = { 0.73f, 0.73f, 0.4f };

	light.pos = { 0.0f, 100.0f, 20.0f };

	Lights.push_back(light);


	for (int i = 0; i < 50; i++)
	{
		TYvec v = randomVec();
		light.ambi = { v * 1.2f };
		light.diff = { v * 0.94f };
		light.spec = { v };

		light.pos = { GetRand(100.0f, 270.0f), GetRand(10.0f, 35.0f), GetRand(-150.0f, 100.0f) };

		Lights.push_back(light);
	}

	//albedoMap =     Material::CreateTexture("./resources/models/Helmet/BaseColor.png");
	//normalMap =     Material::CreateTexture("./resources/models/Helmet/NormalMap.png");
	//metallicMap =   Material::CreateTexture("./resources/models/Helmet/Metalness.png");
	//roughnessMap =  Material::CreateTexture("./resources/models/Helmet/Roughness.png");
	//aoMap =         Material::CreateTexture("./resources/models/Helmet/AOMap.png");
	albedoMap =     Material::CreateTexture("./resources/models/test/albedo.png");
	normalMap =     Material::CreateTexture("./resources/models/test/normal.png");
	metallicMap =   Material::CreateTexture("./resources/models/test/metallic.png");
	roughnessMap =  Material::CreateTexture("./resources/models/test/roughness.png");
	aoMap =         Material::CreateTexture("./resources/models/test/ao.png");
}

RenderDeferred::~RenderDeferred()
{

}