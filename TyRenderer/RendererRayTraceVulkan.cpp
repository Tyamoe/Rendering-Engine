#include "Tyamoe3DHelper.h"

#ifndef TYAMOE3D

#include "stdafx.h"
#include "Globals.h"

#else

#include EngineInc(stdafx.h)
#include EngineInc(Globals.h)

#endif // TYAMOE3D

#include "RendererRayTraceVulkan.h"

TYvoid RenderRayTraceVulkan::PreRender() 
{

}

TYvoid RenderRayTraceVulkan::Render(TYfloat dt) 
{
	Layout layout = window->GetLayout();
	TYint width = layout.width;
	TYint height = layout.height;

	camera->Update(dt);

	GenericDraw::dt = dt;
	GenericDraw::projection = glm::perspective(glm::radians(Global::FOV), TYfloat(width) / height, 0.1f, 1000.0f);;
	GenericDraw::view = camera->view;
}

TYvoid RenderRayTraceVulkan::PostRender() 
{
	QuadShader->Use();
	QuadShader->DrawQuad(RenderTexture);
}

TYvoid RenderRayTraceVulkan::Init()
{
	Layout layout = window->GetLayout();

	TYint width = layout.width;
	TYint height = layout.height;

	camera = new Camera(TYnull, true);

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
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer in Raytrace Renderer is not complete!" << std::endl;
	}

	glGenTextures(1, &Frame);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Frame);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, layout.width, layout.height, 0, GL_RGBA, GL_FLOAT, NULL);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

RenderRayTraceVulkan::RenderRayTraceVulkan() : Renderer()
{
	SetType(RendererType::RayTraceVulkan);

	scene = new Scene();
}

RenderRayTraceVulkan::~RenderRayTraceVulkan()
{

}