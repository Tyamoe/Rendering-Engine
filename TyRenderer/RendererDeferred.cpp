#include "RendererDeferred.h"

#include "Input.h"
#include "Window.h"
#include "Scene.h"
#include "Entity.h"

#include "GenericDraw.h"
#include "Octree.h"

#include "Material.h"
#include "Animation.h"
#include "Transform.h"
#include "Mesh.h"

#include "Camera.h"
#include "Shader.h"

#include "AnimationUtils.h"
#include "Globals.h"
#include "ImGuiUtils.h"
#include "GLUtils.h"
#include "Utils.h"

#define L_POINT 1
#define L_DIR   2
#define L_SPOT  3

struct Light
{
	TYint type;
	TYvec3 ambi = { 1.0f, 0.0f, 0.0f };
	TYvec3 diff = { 0.0f, 1.0f, 0.0f };
	TYvec3 spec = { 0.0f, 0.0f, 1.0f };

	TYvec3 pos = { 0.0f, 0.0f, 0.0f };
	TYfloat c0 = 0.11f;
	TYfloat c1 = 0.15f;
	TYfloat c2 = 0.07f;

	TYvec3 dir;
	TYfloat innerTheta;
	TYfloat outerTheta;
	TYfloat spotIntensity;
};

inline TYvec3 randomVec()
{
	return
	{
		fmodf((TYfloat)rand(), 155.0f) / 255.0f,
		fmodf((TYfloat)rand(), 155.0f) / 255.0f,
		fmodf((TYfloat)rand(), 155.0f) / 255.0f
	};
}

static TYvector<Light> Lights;

static TYbool tempPlay = false;
static TYbool tempDrawOctree = false;
static TYbool tempDrawSkeleton = false;
static TYbool tempDrawLights = false;

static TYuint albedoMap = 0;
static TYuint normalMap = 0;
static TYuint metallicMap = 0;
static TYuint roughnessMap = 0;
static TYuint aoMap = 0;

static TYvec tmpVec = {0, 0, 0};

static TYvec PositionBitss = { 0.0f, 0.0f, 0.0f };
static TYfloat ScaleBithc = 1.0f;
static TYvec4 RotationBatch = { 0.0f, 0.0f, 0.0f, 1.0f };

TYvoid RenderDeferred::Draw(Entity* entity)
{
	Mesh* mesh = entity->Get<Mesh*>();
	Animation* anim = entity->Get<Animation*>();
	Material* mat = mesh->Get<Material*>();

	if (mat->mShader == MaterialShader::Phong)
	{
		Shader* shader = BufferShader;

		if (mesh->IsAnimated())
		{
			shader = animBufferShader;
		}

		shader->Use();

		TYmat modelMatrix = entity->GetMatrix();
		if (mesh->IsAnimated())
		{
			TYmat trMatrix = glm::translate(TYmat(1.0f), PositionBitss);
			TYmat rtMatrix = glm::mat4_cast(TYquaternion(RotationBatch));
			TYmat scMatrix = glm::scale(TYmat(1.0f), TYvec(ScaleBithc));

			modelMatrix = trMatrix * rtMatrix * scMatrix;
		}
		TYmat modelInv = glm::inverse(modelMatrix);
		modelInv = glm::transpose(modelInv);

		TYmat MVP = camera->proj * camera->view * modelMatrix;

		shader->Uniforms["InvTrModel"](modelInv);
		shader->Uniforms["MVP"](MVP);
		shader->Uniforms["Model"](modelMatrix);

		shader->Uniforms["meshColor"](mat->color);

		if (mesh->IsAnimated())
		{
			shader->Uniforms["SkeletonTransform"](anim->currentPose, (TYint)anim->currentPose.size());
		}

		MeshHandle& mHandle = mesh->GetHandle();

		glBindVertexArray(mHandle.VAO);

		for (TYuint i = 0; i < mesh->SubMeshCount(); i++)
		{
			const SubMesh& subMesh = mesh->GetSubMesh(i);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, mat->GetTexture(subMesh.MaterialIndex));

			glDrawElementsBaseVertex(GL_TRIANGLES, subMesh.NumIndices, GL_UNSIGNED_INT,
				(void*)(sizeof(TYuint) * subMesh.OffsetIndex), subMesh.OffsetVertex);
		}

		glBindVertexArray(0);
	}
	else
	{
		Draw(entity, true);
	}
}

TYvoid RenderDeferred::Draw(Entity* entity, TYbool)
{
	Mesh* mesh = entity->Get<Mesh*>();
	Animation* anim = entity->Get<Animation*>();
	Material* mat = mesh->Get<Material*>();

	//if (mat->mShader == MaterialShader::PBR)
	{
		PBRCount++;

		BufferPBRShader->Use();

		TYmat modelMatrix = entity->GetMatrix();
		TYmat MVP = camera->proj * camera->view * modelMatrix;

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

		/*for (TYsizet i = 0; i < mesh->geoCount(); i++)
		{
			MeshHandle& mHandle = mesh->GetHandle((TYint)i);

			glBindVertexArray(mHandle.VAO);
			glDrawElements(GL_TRIANGLES, (TYint)mHandle.indexCount, GL_UNSIGNED_INT, 0);
		}*/

		MeshHandle& mHandle = mesh->GetHandle();

		glBindVertexArray(mHandle.VAO);

		for (TYuint i = 0; i < mesh->SubMeshCount(); i++)
		{
			const SubMesh& subMesh = mesh->GetSubMesh(i);
			glDrawElementsBaseVertex(GL_TRIANGLES, subMesh.NumIndices, GL_UNSIGNED_INT, 
				(void*)(sizeof(TYuint) * subMesh.OffsetIndex), subMesh.OffsetVertex);
		}
  
		glBindVertexArray(0);

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
			MeshHandle& mHandle = mesh->GetHandle((TYint)i);

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

		static TYstring lightUniformCache[128][32] = {""};

		for (TYsizet i = 0; i < Lights.size(); i++)
		{
			if (lightUniformCache[i][0] == "")
			{
				TYstring lname = "lights[" + std::to_string(i) + "].";

				lightUniformCache[i][0] = lname + "type";
				lightUniformCache[i][1] = lname + "ambi";
				lightUniformCache[i][2] = lname + "diff";
				lightUniformCache[i][3] = lname + "spec";
				lightUniformCache[i][4] = lname + "pos";
				lightUniformCache[i][5] = lname + "c0";
				lightUniformCache[i][6] = lname + "c1";
				lightUniformCache[i][7] = lname + "c2";
				lightUniformCache[i][8] = lname + "dir";
				lightUniformCache[i][9] = lname + "innerTheta";
				lightUniformCache[i][10] = lname + "outerTheta";
				lightUniformCache[i][11] = lname + "spotIntensity";
			}

			shader[s]->Uniforms[(TYchar*)(lightUniformCache[i][0]).c_str()](Lights[i].type);
			shader[s]->Uniforms[(TYchar*)(lightUniformCache[i][1]).c_str()](Lights[i].ambi);
			shader[s]->Uniforms[(TYchar*)(lightUniformCache[i][2]).c_str()](Lights[i].diff);
			shader[s]->Uniforms[(TYchar*)(lightUniformCache[i][3]).c_str()](Lights[i].spec);
				   
			shader[s]->Uniforms[(TYchar*)(lightUniformCache[i][4]).c_str()](Lights[i].pos);
			shader[s]->Uniforms[(TYchar*)(lightUniformCache[i][5]).c_str()](Lights[i].c0);
			shader[s]->Uniforms[(TYchar*)(lightUniformCache[i][6]).c_str()](Lights[i].c1);
			shader[s]->Uniforms[(TYchar*)(lightUniformCache[i][7]).c_str()](Lights[i].c2);
				   
			shader[s]->Uniforms[(TYchar*)(lightUniformCache[i][8]).c_str()](Lights[i].dir);
				   
			shader[s]->Uniforms[(TYchar*)(lightUniformCache[i][9]).c_str()](Lights[i].innerTheta);
			shader[s]->Uniforms[(TYchar*)(lightUniformCache[i][10]).c_str()](Lights[i].outerTheta);
			shader[s]->Uniforms[(TYchar*)(lightUniformCache[i][11]).c_str()](Lights[i].spotIntensity);
		}

		shader[s]->DrawQuad();
	}

	glEnable(GL_DEPTH_TEST);
}

TYvoid RenderDeferred::Deferred()
{
	Layout layout = window->GetLayout();

	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	glViewport(0, 0, layout.width, layout.height);
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

		ImGui::Checkbox("Draw Octree", &tempDrawOctree);
		ImGui::Checkbox("Draw Lights", &tempDrawLights);

		/*Transform* t = Entity::Get<Transform*>("Test");

		TYvec p = t->Get<Transformation::Position>();
		TYvec s = t->Get<Transformation::Scale>();

		ImGui::DragFloat3("Light Pos", &p.x, 2.0f);
		ImGui::DragFloat3("Light Scale", &s.x, 0.5f);

		t->Set<Transformation::Position>(p);
		t->Set<Transformation::Scale>(s);

		Lights[1].pos = p;*/

		ImGui::NewLine();

		if (ImGui::Button("Compile Shaders"))
		{
			CompileShaders();
		}

		ImGui::End();

		ImGui::Begin("Test Animations");

		ImGui::SliderFloat3("Position##1337", &PositionBitss.x, -500.0f, 500.0f);
		ImGui::SliderFloat("Scale##1337", &ScaleBithc, 0.0f, 30.0f);
		ImGui::SliderFloat4("Rotation##1337", &RotationBatch.x, -TYpi, TYpi);

		ImGui::End();
	}

	if (tempDrawOctree)
	{
		for (TYuint i = 0; i < scene->entityList.size(); i++)
		{
			Mesh* mesh = scene->entityList[i]->Get<Mesh*>();
			Geometry* g = mesh->GetGeometry();
			if (g->GetType() == geoModel)
			{
				if (((Model*)g)->octree)
				{
					((Model*)g)->octree->Draw(((Model*)g)->octree->root);
				}
			}
		}
	}

	if (tempDrawSkeleton)
	{
		for (TYuint i = 0; i < scene->entityList.size(); i++)
		{
			Animation* anim = scene->entityList[i]->Get<Animation*>();
			if (anim)
			{
				TYmat modelMatrix = scene->entityList[i]->GetMatrix();
				anim->DrawSkeleton(modelMatrix);
				//DrawSkeleton(anim->skeleton->skeleton);
			}
		}
	}

	if (tempDrawLights)
	{
		for (TYint i = 0; i < Lights.size(); i++)
		{
			if (i == 0) continue;
			TYvec center = Lights[i].pos;
			GenericDraw::DrawSphere(center, 0.88f, Lights[i].diff);
		}
	}

	//UI::Use(camera);
	//UI::DrawBackground(Texture2D::CreateTexture("editorBackground.png"), 10.0f);
}

TYvoid RenderDeferred::PreRender()
{

}

static TYvector3 lyons;
static TYvector3 sirkls;
static TYfloat frameTime = 0.0f;
static TYint frameNum = 0;

TYvoid RenderDeferred::Render(TYfloat dt)
{
	Layout layout = window->GetLayout();
	TYint width = layout.width;
	TYint height = layout.height;

	camera->Update(dt);
	
	for (TYuint i = 0; i < scene->entityList.size(); i++)
	{
		Animation* anim = scene->entityList[i]->Get<Animation*>();
		if (anim)
		{
			//UI
			{
				ImGui::Begin("Animation");

				ImGui::Checkbox("Play Animation", &tempPlay);
				ImGui::Checkbox("Draw Skeleton", &tempDrawSkeleton);

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

	if (Input::isMouseDown(MouseRight))
	{
		for (TYuint i = 0; i < scene->entityList.size(); i++)
		{
			Mesh* mesh = scene->entityList[i]->Get<Mesh*>();
			Geometry* g = mesh->GetGeometry();
			if (g->GetType() == geoModel)
			{
				if (((Model*)g)->octree)
				{
					((Model*)g)->octree->Traverse(((Model*)g)->octree->root);
				}
			}
		}
	}
	if (Input::isMouseReleased(MouseMiddle))
	{
		TYfloat y_, x_;

		x_ = width / 2.0f; //Input::mouse.screenPos.x;
		y_ = height / 2.0f; //Input::mouse.screenPos.y;

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

		for (TYuint i = 0; i < scene->entityList.size(); i++)
		{
			TYvec norm = TYvec(0.0f);
			TYfloat t0 = TYinf, t1 = TYinf;

			if (Geometry::Intersect(scene->entityList[i], rayOrigin, rayDir, t0, t1, norm))
			{
				if (t0 < 0)
				{
					t0 = t1;
				}

				if (t0 < tnear)
				{
					Mesh* mesh = scene->entityList[i]->Get<Mesh*>();

					tnear = t0;
					hit = mesh->GetGeometry(-1);// scene->geometry[i];
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
			TYlog << "Hit: " << scene->entityList[hitIndex]->GetName() << "\n";
			//GenericDraw::DrawLine(camera->position, hit->center, TYvec(0), 2.0f);
			lyons.push_back(camera->position);
			lyons.push_back(camera->position + rayDir * tnear);
			//lyons.push_back(hit->center);

			sirkls.push_back(camera->position + rayDir * tnear);
		}
	}

	for (TYsizet i = 0; i < lyons.size(); i+=2)
	{
		GenericDraw::DrawLine(lyons[i], lyons[i+1], TYvec(0), 1.5f);
	}
	for (TYsizet i = 0; i < sirkls.size(); i++)
	{
		GenericDraw::DrawSphere(sirkls[i], 0.05f, TYvec(1, 0, 1));
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

	camera = new Camera(false);

	GenericDraw::MainCamera = camera;

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

	Mesh::GenHandles = true;
	Material::WhiteTexture = Material::CreateTexture(TYvec4(1.0f));

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


	for (TYint i = 0; i < 50; i++)
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
	/*albedoMap =     Material::CreateTexture("./resources/models/test/albedo.png");
	normalMap =     Material::CreateTexture("./resources/models/test/normal.png");
	metallicMap =   Material::CreateTexture("./resources/models/test/metallic.png");
	roughnessMap =  Material::CreateTexture("./resources/models/test/roughness.png");
	aoMap =         Material::CreateTexture("./resources/models/test/ao.png");*/
}

RenderDeferred::~RenderDeferred()
{
	delete animBufferShader;
	delete BufferShader;
	delete PhongShader;
	delete BufferPBRShader;
	delete PBRShader;
	delete QuadShader;
	delete ColorShader;

	glDeleteTextures(1, &RenderTexture);

	glDeleteFramebuffers(1, &RenderBuffer);

	glDeleteTextures(1, &gBuffer);
	glDeleteTextures(1, &gDepth);
	glDeleteTextures(1, &gPosition);
	glDeleteTextures(1, &gNormal);
	glDeleteTextures(1, &gAlbedo);
	glDeleteTextures(1, &gExtra);

	lyons.clear();
	sirkls.clear();
}