#include "RendererRayTraceVulkan.h"

#include "Window.h"

#include "Scene.h"
#include "Shader.h"
#include "Camera.h"

#include "Globals.h"

// https://github.com/GraphicsProgramming/RVPT/blob/master/src/rvpt/rvpt.cpp

TYvoid RenderRayTraceVulkan::PreRender() 
{

}

TYvoid RenderRayTraceVulkan::Render(TYfloat dt) 
{
	Layout layout = window->GetLayout();
	TYint width = layout.width;
	TYint height = layout.height;

	camera->Update(dt);
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

	camera = new Camera(true);

}

RenderRayTraceVulkan::RenderRayTraceVulkan() : Renderer()
{
	SetType(RendererType::RayTraceVulkan);

	Mesh::GenHandles = false;

	scene = new Scene();
}

RenderRayTraceVulkan::~RenderRayTraceVulkan()
{

}