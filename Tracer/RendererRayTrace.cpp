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

}

RenderRayTrace::RenderRayTrace()
{

}

RenderRayTrace::~RenderRayTrace()
{

}