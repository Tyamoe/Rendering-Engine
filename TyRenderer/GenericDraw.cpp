#include "Tyamoe3DHelper.h"

#ifndef TYAMOE3D

#include "stdafx.h"

#else

#include EngineInc(stdafx.h)

#endif // TYAMOE3D

#include "GenericDraw.h"

TYvoid GenericDraw::Init()
{
	ColorShader = new Shader("color.vs", "color.fs");

	// Sphere
	{
		TYvec prev;
		TYvec cur(0.0f);

		for (TYfloat d = 0; d < 360; d += 12)
		{
			prev = cur;
			cur.x = cosf(glm::radians(d));
			cur.z = sinf(glm::radians(d));

			if (d > 0)
			{
				SphereVertices.push_back(prev);
				SphereVertices.push_back(cur);
			}
		}
		SphereVertices.push_back(SphereVertices.back());
		SphereVertices.push_back(SphereVertices.front());

		TYvector3 ye = SphereVertices;
		for (TYint i = 1; i <= 3; i++)
		{
			glm::mat4 rot = glm::rotate(glm::mat4(1), 1.57079632679f * i, TYvec(0, 0, 1));
			for (TYvec p : ye)
				SphereVertices.push_back(rot * glm::vec4(p, 1));
		}

		glm::mat4 rot = glm::rotate(glm::mat4(1), 1.57079632679f, TYvec(1, 0, 0));
		for (TYvec p : ye)
			SphereVertices.push_back(rot * glm::vec4(p, 1));
	}
}

TYvoid GenericDraw::DrawCube(TYvec pos, TYvec size, TYvec color, TYfloat width)
{
	static TYuint wireVao = 0;
	static TYuint wireVbo = 0;
	if (wireVao == 0)
	{
		glGenVertexArrays(1, &wireVao);
		glGenBuffers(1, &wireVbo);
		glBindVertexArray(wireVao);
		glBindBuffer(GL_ARRAY_BUFFER, wireVbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(CubeVertices), &CubeVertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(TYfloat), (TYvoid*)0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	ColorShader->Use();

	TYmat model(1.0f);
	model = glm::translate(model, pos);
	model = glm::scale(model, size * 0.5f);

	ColorShader->setMat4(ColorShader->Uniforms["Model"], model);
	ColorShader->setMat4(ColorShader->Uniforms["View"], view);
	ColorShader->setMat4(ColorShader->Uniforms["Proj"], projection);
	ColorShader->setVec3(ColorShader->Uniforms["oColor"], color);

	glBindVertexArray(wireVao);
	glLineWidth(width);

	glDrawArrays(GL_LINES, 0, 24);

	glBindVertexArray(0);
}

TYvoid GenericDraw::DrawSphere(TYvec pos, TYfloat radius, TYvec color)
{
	static TYuint wireVao = 0;
	static TYuint wireVbo = 0;
	if (wireVao == 0)
	{
		glGenVertexArrays(1, &wireVao);
		glGenBuffers(1, &wireVbo);
		glBindVertexArray(wireVao);
		glBindBuffer(GL_ARRAY_BUFFER, wireVbo);
		glBufferData(GL_ARRAY_BUFFER, SphereVertices.size() * sizeof(TYvec), &SphereVertices[0], GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(TYfloat), (TYvoid*)0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	ColorShader->Use();

	TYmat model(1.0f);
	model = glm::translate(model, pos);
	model = glm::scale(model, TYvec(radius));

	ColorShader->setMat4(ColorShader->Uniforms["Model"], model);
	ColorShader->setMat4(ColorShader->Uniforms["View"], view);
	ColorShader->setMat4(ColorShader->Uniforms["Proj"], projection);
	ColorShader->setVec3(ColorShader->Uniforms["oColor"], color);

	glBindVertexArray(wireVao);

	glDrawArrays(GL_LINES, 0, (GLsizei)SphereVertices.size());

	glBindVertexArray(0);
}
