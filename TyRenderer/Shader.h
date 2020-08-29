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

struct CharPointerCMP
{
	TYbool operator()(TYchar const *a, TYchar const *b) const
	{
		return std::strcmp(a, b) < 0;
	}
};

// Handles all things shader. Creates shader programs and set uniforms.
typedef class Shader
{
public:
	// The program.
	TYuint Program;

	TYmap<TYchar*, TYint, CharPointerCMP> Uniforms;
	//std::map<std::string, TYint> Uniforms;

	// Shader constructor for simple frag/vertex shader program.
	Shader(TYstring computePath);
	Shader(TYstring vertexPath, TYstring fragmentPath);
	Shader()
	{
		Program = 0;
	}
	~Shader();
	// Tell OpenGL to use 'this' program.
	void Use();

	void SetupUniforms();

	// Functions used to set shader uniforms. (name = name of uniform).
	void setBool(TYint uniformLoc, TYbool value);
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
	void setMat4(TYint uniformLoc, const TYmat &mat);

	TYvoid DrawQuad(TYuint texture, TYbool invert = false)
	{
		static unsigned int quadVAO = 0;
		static unsigned int quadVBO;
		static unsigned int quadVAO1 = 0;
		static unsigned int quadVBO1;
		if (quadVAO == 0)
		{
			float quadVertices[] =
			{
				// positions        // texture Coords
				-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
				-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
				 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
				 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
			};
			float quadVertices2[] =
			{
				// positions        // texture Coords
				-1.0f,  1.0f, 0.0f, 0.0f, 0.0f,
				-1.0f, -1.0f, 0.0f, 0.0f, 1.0f,
				 1.0f,  1.0f, 0.0f, 1.0f, 0.0f,
				 1.0f, -1.0f, 0.0f, 1.0f, 1.0f,
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

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);

			glGenVertexArrays(1, &quadVAO1);
			glGenBuffers(1, &quadVBO1);
			glBindVertexArray(quadVAO1);
			glBindBuffer(GL_ARRAY_BUFFER, quadVBO1);
			glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices2), &quadVertices2, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
		}

		setInt(Uniforms["texture1"], 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);

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