#include "stdafx.h"

#include <random>

#include "RendererRayTraceCPU.h"

union Color
{
	Color(GLubyte red = 0, GLubyte green = 0, GLubyte blue = 0, GLubyte alpha = 255) :
		r(red), g(green), b(blue), a(alpha) {}
	Color(GLubyte all, bool t) // for z buffer
	{
		r = g = b = all;
		a = 255;
	}

	struct
	{
		GLubyte r, g, b, a;
	};

	GLubyte val[4];

	GLuint raw;
};

GLuint pboid;

static typename Color* ptr_to_pbo;

TYvoid RenderRayTraceCPU::RandomBS()
{
	Layout layout = engine->GetWindow()->GetLayout();

	GLsizei pixel_cnt = layout.width * layout.height;
	GLsizei byte_cnt = pixel_cnt * sizeof(Color);

	glCreateTextures(GL_TEXTURE_2D, 1, &RenderTexture);
	glTextureStorage2D(RenderTexture, 1, GL_RGBA8, layout.width, layout.height);

	glCreateBuffers(1, &pboid);
	glNamedBufferStorage(pboid, byte_cnt, NULL, GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT);

	ptr_to_pbo = (Color*)glMapNamedBuffer(pboid, GL_WRITE_ONLY);

	std::fill(ptr_to_pbo, ptr_to_pbo + pixel_cnt, Color(122, 155, 66, 255));

	glUnmapNamedBuffer(pboid);

	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboid);

	glTextureSubImage2D(GL_TEXTURE_2D, 0, 0, 0, layout.width, layout.height, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
}


TYvoid RenderRayTraceCPU::PreRender()
{

}

TYvoid DrawQuad(ShaderPtr QuadShader, TYuint texture)
{
	static unsigned int quadVAO = 0;
	static unsigned int quadVBO;
	if (quadVAO == 0)
	{
		float quadVertices[] =
		{
			// positions        // texture Coords
			-0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
			 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
			 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
	}

	QuadShader->setInt(QuadShader->Uniforms["texture1"], 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

TYvoid RenderRayTraceCPU::Render(TYfloat dt)
{
	Layout layout = engine->GetWindow()->GetLayout();

	/*for (int x = 0; x < layout.width; x++)
	{
		for (int y = 0; y < layout.width; y++)
		{
			PixelBuffer[x * layout.width + y] = TYvec(1, 1, 1);
		}
	}*/

	/*glBindFramebuffer(GL_FRAMEBUFFER, RenderBuffer);
	glClearColor(0.3f, 0.69f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, RenderTexture);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, layout.width, layout.height, GL_RGB, GL_FLOAT, &PixelBuffer[0].x);*/

	/*glBindTexture(GL_TEXTURE_2D, RenderTexture);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboid);

	ptr_to_pbo = (Color*)glMapNamedBuffer(pboid, GL_WRITE_ONLY);

	std::fill(ptr_to_pbo, ptr_to_pbo + (layout.width * layout.height), Color(122, 155, 66, 255));


	glUnmapNamedBuffer(pboid);

	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, layout.width, layout.height, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);*/

	glBindTexture(GL_TEXTURE_2D, RenderTexture);
	float a[] = { 1.0f, 1.0f , 1.0f , 1.0f };
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 1, 1, GL_RGBA, GL_FLOAT, a);

	QuadShader->Use();
	DrawQuad(QuadShader, RenderTexture);
}

TYvoid RenderRayTraceCPU::PostRender()
{

}

TYuint RenderRayTraceCPU::AddMesh(Mesh& mesh)
{
	TYuint offset = Scene.size();
	Scene += mesh;

	return offset;
}

TYvoid RenderRayTraceCPU::Init()
{
	Layout layout = engine->GetWindow()->GetLayout();

	QuadShader = new Shader("quad.vs", "quad.fs");

	//PixelBuffer = TYvector3(layout.width * layout.height, TYvec(1, 1, 1));

	//RandomBS();

	float a[] = { 1.0f, 1.0f , 1.0f , 1.0f };
	// Texture
	glCreateTextures(GL_TEXTURE_2D, 1, &RenderTexture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 1, 1, 0, GL_RGBA, GL_FLOAT, a);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		/**/
	// FrameBuffer
	/*glGenFramebuffers(1, &RenderBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, RenderBuffer);

	glGenTextures(1, &RenderTexture);
	glBindTexture(GL_TEXTURE_2D, RenderTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, layout.width, layout.height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, RenderTexture, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer in Raytrace Renderer is not complete!" << std::endl;
	}*/

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

RenderRayTraceCPU::RenderRayTraceCPU()
{

}

RenderRayTraceCPU::~RenderRayTraceCPU()
{

}