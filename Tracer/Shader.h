#pragma once

#include "stdafx.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <map>

struct CharPointerCMP
{
	bool operator()(char const *a, char const *b) const
	{
		return std::strcmp(a, b) < 0;
	}
};

// Handles all things shader. Creates shader programs and set uniforms.
typedef class Shader
{
public:
	// The program.
	GLuint Program;

	std::map<GLchar*, GLint, CharPointerCMP> Uniforms;
	//std::map<std::string, GLint> Uniforms;

	// Shader constructor for simple frag/vertex shader program.
	Shader(std::string vertexPath, std::string fragmentPath);
	Shader()
	{
		Program = 0;
	}
	~Shader();
	// Tell OpenGL to use 'this' program.
	void Use();

	// Functions used to set shader uniforms. (name = name of uniform).
	void setBool(GLint uniformLoc, bool value);
	void setInt(GLint uniformLoc, int value);
	void setFloat(GLint uniformLoc, float value);

	void set3FloatArray(GLint uniformLoc, const float* value, int count);
	void set4FloatArray(GLint uniformLoc, const float* value, int count);

	void setIntArray(GLint uniformLoc, std::vector<int>& value, int count);

	void set1FloatArray(GLint uniformLoc, std::vector<float>& value, int count);
	void set2FloatArray(GLint uniformLoc, std::vector<glm::vec2>& value, int count);
	void set3FloatArray(GLint uniformLoc, std::vector<glm::vec3>& value, int count);
	void set4FloatArray(GLint uniformLoc, std::vector<glm::vec4>& value, int count);

	void setVec2(GLint uniformLoc, const glm::vec2 &value);
	void setVec2(GLint uniformLoc, float x, float y);
	void setVec3(GLint uniformLoc, const glm::vec3 &value);
	void setVec3(GLint uniformLoc, float x, float y, float z);
	void setVec4(GLint uniformLoc, const glm::vec4 &value);
	void setVec4(GLint uniformLoc, float x, float y, float z, float w);
	
	void setMat3(GLint uniformLoc, const glm::mat3 &mat);
	void setMat4(GLint uniformLoc, const glm::mat4 &mat);

}Shader, *ShaderPtr;