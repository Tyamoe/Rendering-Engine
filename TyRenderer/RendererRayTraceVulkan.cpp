#include "RendererRayTraceVulkan.h"

#include "Window.h"

#include "Scene.h"
#include "Shader.h"
#include "Camera.h"

#include "Globals.h"

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

	scene = new Scene();
}

RenderRayTraceVulkan::~RenderRayTraceVulkan()
{

}