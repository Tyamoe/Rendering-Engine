#pragma once

#include "Types.h"

class Camera;
class Shader;

class GenericDraw
{
public:
	static TYvoid Init();
	static TYvoid Destroy();
	static TYvoid Update(TYfloat dt);

	static TYvoid DrawCube(TYvec pos, TYvec size, TYvec color, TYfloat width = 2.0f);

	static TYvoid DrawSphere(TYvec pos, TYfloat radius, TYvec color, TYfloat width = 2.0f);

	static TYvoid DrawLine(TYvec start, TYvec end, TYvec color, TYfloat width = 2.0f);

	static TYvoid DrawQuad(TYvec2 pos, TYvec2 size, TYuint texture);
	static TYvoid DrawQuad(TYvec2 pos, TYvec2 size, TYvec color);

	static TYvoid DrawCircle(TYvec2 pos, TYfloat size, TYuint texture);
	static TYvoid DrawCircle(TYvec2 pos, TYfloat size, TYvec color);

	static TYvoid DrawFSQuad(TYuint texture);

	static TYvoid DrawText(TYstring text, TYvec2 pos, TYfloat scale, TYvec4 color);
	static TYvoid DrawText(TYstring text, TYvec pos, TYfloat scale, TYvec4 color);

	friend class Camera;

	static inline TYvector<TYpair<TYvec, TYfloat>> RecurringSpheres;

	static inline const Camera* MainCamera = TYnull;
private:

	static inline Shader* ColorShader = TYnull;
	static inline Shader* Color2DShader = TYnull;
	static inline Shader* TextureShader = TYnull;
	static inline Shader* Texture2DShader = TYnull;
	static inline Shader* TextShader = TYnull;

	static inline TYuint vboCubeHandle;
	static inline TYuint CubeHandle;
	static inline TYuint SphereHandle;
	static inline TYuint QuadHandle;
	static inline TYuint QuadUVHandle;
	static inline TYuint CircleHandle;
	static inline TYuint CircleUVHandle;

	static inline TYuint LineVAO = 0;
	static inline TYuint LineVBO = 0;

	static inline TYuint fsQuadHandle;
};

inline const TYfloat FSQuadVertices[] =
{
	-1.0f,  1.0f, 0.0f, 1.0f,
	-1.0f, -1.0f, 0.0f, 0.0f,
	1.0f,  1.0f, 1.0f, 1.0f,
	1.0f, -1.0f, 1.0f, 0.0f,
};

inline const TYfloat QuadVertices2[] =
{
	0.5f,  0.5f, 
	0.5f, -0.5f, 
	-0.5f, -0.5f,
	-0.5f,  0.5f
};

inline const TYfloat QuadVertices[] =
{
	0.5f,  0.5f,  1.0f, 1.0f,
	0.5f, -0.5f,  1.0f, 0.0f,
	-0.5f, -0.5f, 0.0f, 0.0f,
	-0.5f,  0.5f, 0.0f, 1.0f

	//0.5f,  0.5f,  1.0f, 0.0f,
	//0.5f, -0.5f,  1.0f, 1.0f,
	//-0.5f, -0.5f, 0.0f, 1.0f,
	//-0.5f,  0.5f, 0.0f, 0.0f
};

inline const TYuint QuadIndices[] =
{
	0, 1, 3,
	1, 2, 3
};

inline const TYfloat CubeVertices[] =
{
	-0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f, -0.5f,
	-0.5f, -0.5f,  0.5f,
	 0.5f, -0.5f,  0.5f,
	-0.5f, -0.5f, -0.5f,
	-0.5f, -0.5f,  0.5f,
	 0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f,  0.5f,
	-0.5f,  0.5f, -0.5f,
	 0.5f,  0.5f, -0.5f,
	-0.5f,  0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f, -0.5f,
	-0.5f,  0.5f,  0.5f,
	 0.5f,  0.5f, -0.5f,
	 0.5f,  0.5f,  0.5f,
	-0.5f, -0.5f, -0.5f,
	-0.5f,  0.5f, -0.5f,
	-0.5f, -0.5f,  0.5f,
	-0.5f,  0.5f,  0.5f,
	 0.5f, -0.5f, -0.5f,
	 0.5f,  0.5f, -0.5f,
	 0.5f, -0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,
};