#pragma once

#include "Types.h"

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
	Shader(TYstring vertexPath, TYstring fragmentPath);
	Shader()
	{
		Program = 0;
	}
	~Shader();
	// Tell OpenGL to use 'this' program.
	void Use();

	// Functions used to set shader uniforms. (name = name of uniform).
	void setBool(TYint uniformLoc, TYbool value);
	void setInt(TYint uniformLoc, TYint value);
	void setFloat(TYint uniformLoc, TYfloat value);

	void set3FloatArray(TYint uniformLoc, const TYfloat* value, TYint count);
	void set4FloatArray(TYint uniformLoc, const TYfloat* value, TYint count);

	void setIntArray(TYint uniformLoc, TYvectori& value, TYint count);

	void set1FloatArray(TYint uniformLoc, TYvectorf& value, TYint count);
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

}Shader, *ShaderPtr;