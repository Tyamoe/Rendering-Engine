#pragma once

#ifndef TYAMOE3D

#include "Types.h"

#else

#include "Tyamoe3DHelper.h"
#include EngineInc(Types.h)

#endif // TYAMOE3D

#include <fstream>
#include <sstream>
#include <iostream>

struct CharPointerHash
{
	//BKDR hash algorithm
	TYint operator()(TYchar* str) const
	{
		TYint seed = 131;//31  131 1313 13131131313 etc//
		TYint hash = 0;
		while (*str)
		{
			hash = (hash * seed) + (*str);
			str++;
		}
		return hash & (0x7FFFFFFF);
	}
};

struct CharPointerCMP
{
	TYbool operator()(TYchar const* a, TYchar const* b) const
	{
		return std::strcmp(a, b) == 0;
	}
};

struct UniformSetter
{
	TYvoid operator()(TYint value);
	TYvoid operator()(TYbool value);
	TYvoid operator()(TYfloat value);

	TYvoid operator()(TYvec2 value);
	TYvoid operator()(TYvec value);
	TYvoid operator()(TYvec4 value);

	TYvoid operator()(TYmat3 value);
	TYvoid operator()(TYmat value);

	TYvoid operator()(TYvectorI& value, TYint count);
	TYvoid operator()(TYvectorF& value, TYint count);

	TYvoid operator()(TYvector3& value, TYint count);
	TYvoid operator()(TYvector<TYmat>& value, TYint count);

	TYint loc = -1;
	TYint type = -1;
};

typedef class Shader
{
public:
	TYuint Program;

	TYumap<TYchar*, UniformSetter, CharPointerHash, CharPointerCMP> Uniforms;

	Shader(TYstring computePath);
	Shader(TYstring vertexPath, TYstring fragmentPath);
	Shader()
	{
		Program = 0;
	}
	~Shader();

	friend struct UniformSetter;

	TYvoid Use();

	TYvoid SetupUniforms();

	/*void setBool(TYint uniformLoc, TYbool value);
	void setInt(TYint uniformLoc, TYint value);
	void setFloat(TYint uniformLoc, TYfloat value);

	void set3FloatArray(TYint uniformLoc, const TYfloat* value, TYint count);
	void set4FloatArray(TYint uniformLoc, const TYfloat* value, TYint count);

	void setIntArray(TYint uniformLoc, TYvectorI& value, TYint count);

	void set1FloatArray(TYint uniformLoc, TYvectorF& value, TYint count);
	void set2FloatArray(TYint uniformLoc, TYvector<TYvec2>& value, TYint count);
	void set3FloatArray(TYint uniformLoc, TYvector3& value, TYint count);
	void set4FloatArray(TYint uniformLoc, TYvector<TYvec4>& value, TYint count);

	void setVec2(TYint uniformLoc, const TYvec2 &value);
	void setVec2(TYint uniformLoc, TYfloat x, TYfloat y);
	void setVec3(TYint uniformLoc, const TYvec &value);
	void setVec3(TYint uniformLoc, TYfloat x, TYfloat y, TYfloat z);
	void setVec4(TYint uniformLoc, const TYvec4 &value);
	void setVec4(TYint uniformLoc, TYfloat x, TYfloat y, TYfloat z, TYfloat w);
	
	void setMat3(TYint uniformLoc, const TYmat3 &mat);
	void setMat4(TYint uniformLoc, const TYmat &mat);*/

	TYvoid DrawQuad(TYint texture = -1, TYbool invert = false)
	{
		static TYuint quadVAO = 0;
		static TYuint quadVBO;
		static TYuint quadVAO1 = 0;
		static TYuint quadVBO1;
		if (quadVAO == 0)
		{
			TYfloat quadVertices[] =
			{
				-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
				-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
				 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
				 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
			};
			TYfloat quadVertices2[] =
			{
				-1.0f,  1.0f, 0.0f, 0.0f, 0.0f,
				-1.0f, -1.0f, 0.0f, 0.0f, 1.0f,
				 1.0f,  1.0f, 0.0f, 1.0f, 0.0f,
				 1.0f, -1.0f, 0.0f, 1.0f, 1.0f,
			};

			glGenVertexArrays(1, &quadVAO);
			glGenBuffers(1, &quadVBO);
			glBindVertexArray(quadVAO);
			glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
			
			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(TYfloat), (void*)0);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(TYfloat), (void*)(3 * sizeof(TYfloat)));

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);

			glGenVertexArrays(1, &quadVAO1);
			glGenBuffers(1, &quadVBO1);
			glBindVertexArray(quadVAO1);
			glBindBuffer(GL_ARRAY_BUFFER, quadVBO1);
			glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices2), &quadVertices2, GL_STATIC_DRAW);

			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(TYfloat), (void*)0);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(TYfloat), (void*)(3 * sizeof(TYfloat)));
			
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
		}

		if (texture != -1)
		{
			Uniforms["texture1"](0);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, (TYuint)texture);
		}

		if (invert)
		{
			glBindVertexArray(quadVAO1);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		}
		else
		{
			glBindVertexArray(quadVAO);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		}

		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

}Shader, *ShaderPtr;