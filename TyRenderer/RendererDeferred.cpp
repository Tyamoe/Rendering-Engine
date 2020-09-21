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

#include "RendererDeferred.h"

static TYvector<Light> Lights;

void Quading()
{
	static TYuint quadVAO = 0;
	static TYuint quadVBO;
	if (quadVAO == 0)
	{
		TYfloat quadVertices[] =
		{
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};

		glGenVertexArrays(1, &quadVAO);
		glBindVertexArray(quadVAO);

		glGenBuffers(1, &quadVBO);

		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(TYfloat), (TYvoid*)0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(TYfloat), (TYvoid*)(3 * sizeof(TYfloat)));
	}

	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0); 
}

void RenderDeferred::Draw(Geometry* geometry)
{
	BufferShader->Use();

	TYmat modelMatrix = geometry->GetMatrix();
	TYmat modelInv = glm::inverse(modelMatrix);
	modelInv = glm::transpose(modelInv);
	TYmat MVP = GenericDraw::projection * camera->view * modelMatrix;

	BufferShader->setMat4(BufferShader->Uniforms["MVP"], MVP);
	BufferShader->setMat4(BufferShader->Uniforms["Model"], modelMatrix);
	BufferShader->setMat4(BufferShader->Uniforms["InvTrModel"], modelInv);
	BufferShader->setVec4(BufferShader->Uniforms["meshColor"], TYvec4(0.75f, 0.76f, 0.75f, 1.0f));

	glBindVertexArray(geometry->meshHandle.VAO);
	glDrawElements(GL_TRIANGLES, (TYint)geometry->meshHandle.indexCount, GL_UNSIGNED_INT, 0);
}

TYvoid RenderDeferred::LightPass()
{
	PhongShader->Use();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);

	PhongShader->setInt(PhongShader->Uniforms["positionTexture"], 0);
	PhongShader->setInt(PhongShader->Uniforms["normalTexture"], 1);
	PhongShader->setInt(PhongShader->Uniforms["colorTexture"], 2);

	PhongShader->setInt(PhongShader->Uniforms["num_active_lights"], (TYint)Lights.size());
	PhongShader->setVec3(PhongShader->Uniforms["viewPos"], camera->position);
	PhongShader->setVec3(PhongShader->Uniforms["ambient"], { 0.1f, 0.1f, 0.1f });
	PhongShader->setVec3(PhongShader->Uniforms["fog"], { 0.6f, 0.6f, 0.6f });
	PhongShader->setFloat(PhongShader->Uniforms["ZNear"], 0.1f);
	PhongShader->setFloat(PhongShader->Uniforms["ZFar"], 1000.0f);

	for (TYsizet i = 0; i < Lights.size(); i++)
	{
		TYstring lname = "lights[" + std::to_string(i) + "]";
		PhongShader->setInt(PhongShader->Uniforms  [(TYchar*)(lname + ".type").c_str()], Lights[i].type);
		PhongShader->setVec3(PhongShader->Uniforms [(TYchar*)(lname + ".ambi").c_str()], Lights[i].ambi);
		PhongShader->setVec3(PhongShader->Uniforms [(TYchar*)(lname + ".diff").c_str()], Lights[i].diff);
		PhongShader->setVec3(PhongShader->Uniforms [(TYchar*)(lname + ".spec").c_str()], Lights[i].spec);

		PhongShader->setVec3(PhongShader->Uniforms [(TYchar*)(lname + ".pos").c_str() ], Lights[i].pos);
		PhongShader->setFloat(PhongShader->Uniforms[(TYchar*)(lname + ".c0").c_str()  ], Lights[i].c0);
		PhongShader->setFloat(PhongShader->Uniforms[(TYchar*)(lname + ".c1").c_str()  ], Lights[i].c1);
		PhongShader->setFloat(PhongShader->Uniforms[(TYchar*)(lname + ".c2").c_str()  ], Lights[i].c2);

		PhongShader->setVec3(PhongShader->Uniforms [(TYchar*)(lname + ".dir").c_str() ], Lights[i].dir);

		PhongShader->setFloat(PhongShader->Uniforms[(TYchar*)(lname + ".innerTheta").c_str()], Lights[i].innerTheta);
		PhongShader->setFloat(PhongShader->Uniforms[(TYchar*)(lname + ".outerTheta").c_str()], Lights[i].outerTheta);
		PhongShader->setFloat(PhongShader->Uniforms[(TYchar*)(lname + ".spotIntensity").c_str()], Lights[i].spotIntensity);
	}

	glDisable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Quading();

	glEnable(GL_DEPTH_TEST);
}

TYvoid RenderDeferred::Deferred()
{
	Layout layout = window->GetLayout();

	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	glEnable(GL_FRAMEBUFFER_SRGB); // gamma correction
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);

	for (TYuint i = 0; i < scene->geometry.size(); i++)
	{
		Draw(scene->geometry[i]);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, RenderBuffer);
	glViewport(0, 0, layout.width, layout.height);
	glClearColor(0.8f, 0.15f, 0.15f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	LightPass();
}

TYvoid RenderDeferred::Forward()
{
	ImGui::Begin("Details");

	ImGui::Text("Camera Pos: %.3f, %.3f, %.3f", camera->position.x, camera->position.y, camera->position.z);
	ImGui::Text("Camera Front: %.3f, %.3f, %.3f", camera->front.x, camera->front.y, camera->front.z);

	ImGui::End();
}

TYvoid RenderDeferred::PreRender()
{

}

TYvoid RenderDeferred::Render(TYfloat dt)
{
	Layout layout = window->GetLayout();
	TYint width = layout.width;
	TYint height = layout.height;

	camera->Update(dt);

	GenericDraw::dt = dt;
	GenericDraw::projection = glm::perspective(glm::radians(Global::FOV), TYfloat(width) / height, 0.1f, 1000.0f);;
	GenericDraw::view = camera->view;

	glEnable(GL_FRAMEBUFFER_SRGB); // gamma correction

	Deferred();

	glDisable(GL_FRAMEBUFFER_SRGB);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, RenderBuffer);
	glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_FRAMEBUFFER_SRGB);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, RenderBuffer);

	Forward();

	glDisable(GL_FRAMEBUFFER_SRGB);
}

TYvoid RenderDeferred::PostRender()
{
	QuadShader->Use();
	QuadShader->DrawQuad(RenderTexture);
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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

	glGenTextures(1, &gAlbedoSpec);
	glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);

	glGenTextures(1, &gDepth);
	glBindTexture(GL_TEXTURE_2D, gDepth);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, gDepth, 0);

	unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, attachments);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

RenderDeferred::RenderDeferred() : Renderer()
{
	SetType(RendererType::Deferred);

	BufferShader = new Shader("deferredBuffer.vs", "deferredBuffer.fs");
	PhongShader = new Shader("deferredPhong.vs", "deferredPhong.fs");
	QuadShader = new Shader("quad.vs", "quad.fs");

	scene = new Scene();
	scene->GenHandles_GL();

	Light light;
	light.type = L_POINT;
	light.ambi = { 0.3f, 0.48f, 0.0f };
	light.diff = { 0.65f, 0.95f, 0.1f };
	light.spec = { 0.77f, 0.86f, 0.7f };
	light.pos = { 0.0f, 30.0f, 0.0f };
	light.c0 = 0.2f;
	light.c1 = 0.1f;
	light.c2 = 0.05f;
	light.dir = { 0.1f, -1.0f, -0.11f };
	light.innerTheta = 11.0f;
	light.outerTheta = 20.0f;
	light.spotIntensity = 5.0f;

	Lights.push_back(light);
}

RenderDeferred::~RenderDeferred()
{

}