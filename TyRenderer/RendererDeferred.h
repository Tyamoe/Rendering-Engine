#pragma once

#include "Types.h"

#include "Renderer.h"
#include "AnimationTemp.h"

class Shader;
class Entity;

class RenderDeferred : public Renderer
{
public:
	RenderDeferred();
	~RenderDeferred();

	TYvoid Init();

	TYvoid PreRender();
	TYvoid Render(TYfloat dt);
	TYvoid PostRender();

	SkinnedMesh* skinnyMesh = TYnull;

private:
	Shader* animBufferShader = TYnull;
		  
	Shader* BufferShader = TYnull;
	Shader* PhongShader = TYnull;
		  
	Shader* BufferPBRShader = TYnull;
	Shader* PBRShader = TYnull;
		  
	Shader* QuadShader = TYnull;
	Shader* ColorShader = TYnull;

	TYuint RenderBuffer;
	TYuint RenderTexture;

	TYuint gBuffer = -1;
	
	TYuint gDepth = -1;
	
	TYuint gPosition = -1;
	TYuint gNormal = -1;
	TYuint gAlbedo = -1;
	TYuint gExtra = -1;

	TYint PBRCount = 0;

	TYvoid Draw(TYbool);
	TYvoid Draw(Entity*);
	TYvoid Draw(Entity*, TYbool);
	TYvoid LightPass();
	TYvoid Deferred();
	TYvoid Forward();

	TYvoid CompileShaders();
};