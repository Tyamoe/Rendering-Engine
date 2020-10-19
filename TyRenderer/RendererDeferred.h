#pragma once

#ifndef TYAMOE3D

#include "Types.h"
#include "Scene.h"

#else

#include "Tyamoe3DHelper.h"
#include EngineInc(Types.h)
#include EngineInc(Scene.h)

#endif // TYAMOE3D

#include "Renderer.h"
#include "Shader.h"
#include "Mesh.h"

#define L_POINT 1
#define L_DIR   2
#define L_SPOT  3

struct Light
{
	int type;
	glm::vec3 ambi = { 1.0f, 0.0f, 0.0f };
	glm::vec3 diff = { 0.0f, 1.0f, 0.0f };
	glm::vec3 spec = { 0.0f, 0.0f, 1.0f };

	glm::vec3 pos = { 0.0f, 0.0f, 0.0f };
	float c0 = 0.11f;
	float c1 = 0.15f;
	float c2 = 0.07f;

	glm::vec3 dir;
	float innerTheta;
	float outerTheta;
	float spotIntensity;
};

inline glm::vec3 randomVec() 
{
	return
	{
		fmodf((float)rand(), 155.0f) / 255.0f,
		fmodf((float)rand(), 155.0f) / 255.0f, 
		fmodf((float)rand(), 155.0f) / 255.0f 
	};
}

class RenderDeferred : public Renderer
{
public:
	RenderDeferred();
	~RenderDeferred();

	TYvoid Init();

	TYvoid PreRender();
	TYvoid Render(TYfloat dt);
	TYvoid PostRender();

private:
	ShaderPtr animBufferShader = TYnull;

	ShaderPtr BufferShader = TYnull;
	ShaderPtr PhongShader = TYnull;

	ShaderPtr BufferPBRShader = TYnull;
	ShaderPtr PBRShader = TYnull;

	ShaderPtr QuadShader = TYnull;

	TYuint RenderBuffer;
	TYuint RenderTexture;

	GLuint gBuffer = -1;

	GLuint gDepth = -1;

	GLuint gPosition = -1;
	GLuint gNormal = -1;
	GLuint gAlbedo = -1;
	GLuint gExtra = -1;

	TYint PBRCount = 0;

	TYvoid Draw(Entity*);
	TYvoid Draw(Entity*, TYbool);
	TYvoid LightPass();
	TYvoid Deferred();
	TYvoid Forward();

	TYvoid CompileShaders();
};

typedef RenderDeferred* RenderDeferredPtr;