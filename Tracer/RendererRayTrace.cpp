#include "stdafx.h"

#include "RendererRayTrace.h"

TYvoid RenderRayTrace::PreRender() 
{

}

TYvoid RenderRayTrace::Render(TYfloat dt) 
{

}

TYvoid RenderRayTrace::PostRender() 
{

}

TYuint RenderRayTrace::AddMesh(Mesh& mesh)
{
	TYuint offset = Scene.size();
	Scene += mesh;

	return offset;
}

TYvoid RenderRayTrace::Init()
{
	RayTraceShader = new Shader("raytrace.cps");
	QuadShader = new Shader("quad.vs", "quad.fs");

	Layout layout = engine->GetWindow()->GetLayout();

	// Texture
	glCreateTextures(GL_TEXTURE_2D, 1, &RenderTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, layout.width, layout.height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// FrameBuffer
	glGenFramebuffers(1, &RenderBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, RenderBuffer);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, RenderTexture, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer in Raytrace Renderer is not complete!" << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

RenderRayTrace::RenderRayTrace()
{

}

RenderRayTrace::~RenderRayTrace()
{

}