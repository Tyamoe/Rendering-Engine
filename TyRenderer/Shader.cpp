#include "Tyamoe3DHelper.h"

#ifndef TYAMOE3D

#include "stdafx.h"

#else

#include EngineInc(stdafx.h)

#endif // TYAMOE3D

#include "Shader.h"

static TYstring path = "resources/shaders/";

Shader::~Shader()
{
	glDeleteProgram(Program);
}

Shader::Shader(TYstring vertexPath, TYstring fragmentPath)
{
	vertexPath = path + vertexPath;
	fragmentPath = path + fragmentPath;

	TYstring vertexCode;
	TYstring fragmentCode;

	std::ifstream vertexFile;
	std::ifstream fragmentFile;

	std::stringstream vertexStream;
	std::stringstream fragmentStream;

	vertexFile.exceptions(std::ifstream::badbit);
	fragmentFile.exceptions(std::ifstream::badbit);

	try
	{
		vertexFile.open(vertexPath);
		fragmentFile.open(fragmentPath);

		vertexStream << vertexFile.rdbuf();
		fragmentStream << fragmentFile.rdbuf();

		vertexFile.close();
		fragmentFile.close();

		vertexCode = vertexStream.str();
		fragmentCode = fragmentStream.str();		
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "Files: " << vertexPath << ", " << fragmentPath << std::endl;
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << e.what() << std::endl;
	}

	const TYchar* vShaderCode = vertexCode.c_str();
	const TYchar* fShaderCode = fragmentCode.c_str();

	TYuint vertex, fragment;
	TYint success;
	TYchar infoLog[512];

	vertex = glCreateShader(GL_VERTEX_SHADER);

	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);

	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		std::cout << "File: " << vertexPath << std::endl;
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	fragment = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);

	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		std::cout << "File: " << fragmentPath << std::endl;
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// Link shaders
	Program = glCreateProgram();

	glAttachShader(Program, vertex);
	glAttachShader(Program, fragment);
	glLinkProgram(Program);

	// Check for linking errors
	glGetProgramiv(Program, GL_LINK_STATUS, &success);
	if (!success) 
	{
		glGetProgramInfoLog(Program, 512, NULL, infoLog);
		std::cout << "Files: " << vertexPath << ", " << fragmentPath << std::endl;
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	glDeleteShader(vertex);
	glDeleteShader(fragment);

	// Setup Uniform Map
	{
		TYint max_length;
		glGetProgramiv(Program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_length);
		TYint num_uniforms;

		TYchar* pname = new TYchar[max_length];

		glGetProgramiv(Program, GL_ACTIVE_UNIFORMS, &num_uniforms);

		for (TYint i = 0; i < num_uniforms; ++i) 
		{
			GLsizei written;
			TYint size;
			GLenum type;
			
			glGetActiveUniform(Program, i, max_length, &written, &size, &type, pname);

			TYchar* pname1 = new TYchar[max_length];

			std::strcpy(pname1, pname);

			if (size > 1)
			{
				pname1[written - 3] = '\0';
			}

			TYint loc = glGetUniformLocation(Program, pname1);

			Uniforms.insert(std::pair<TYchar*, TYint>(pname1, loc));
		}

		delete[] pname;
	}
}

Shader::Shader(TYstring pComputePath)
{
	Program = glCreateProgram();
	GLuint cs = glCreateShader(GL_COMPUTE_SHADER);
	
	std::string computePath = path + pComputePath;

	std::string computeCode;
	std::ifstream computeFile;
	std::stringstream computeStream;

	computeFile.exceptions(std::ifstream::badbit);

	try
	{
		computeFile.open(computePath);
		computeStream << computeFile.rdbuf();
		computeFile.close();
		computeCode = computeStream.str();

		//TYlog << "Shader: " << pComputePath << " successfully opened for reading" << TYlogbreak;

	}
	catch (std::ifstream::failure e)
	{
		std::cout << "Files: " << pComputePath << std::endl;
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << e.what() << std::endl;
		TYlog << std::string(e.what()) << TYlogbreak;
	}

	const GLchar* vShaderCode = computeCode.c_str();

	glShaderSource(cs, 1, &vShaderCode, NULL);
	glCompileShader(cs);
	int rvalue;
	GLchar log[10240];
	glGetShaderiv(cs, GL_COMPILE_STATUS, &rvalue);
	if (!rvalue)
	{
		fprintf(stderr, "\n\nError in compiling the compute shader\n\n");
		GLsizei length;
		glGetShaderInfoLog(cs, 10239, &length, log);
		fprintf(stderr, "Compiler log:\n%s\n", log);
	}
	else
	{
		//TYlog << "Shader: " << pComputePath << " successfully COMPILED" << TYlogbreak;
	}
	glAttachShader(Program, cs);

	glLinkProgram(Program);
	glGetProgramiv(Program, GL_LINK_STATUS, &rvalue);

	if (!rvalue) 
	{
		fprintf(stderr, "Error in linking compute shader program\n\n");
		GLsizei length;
		glGetProgramInfoLog(Program, 10239, &length, log);
		fprintf(stderr, "\nLinker log:\n%s\n", log);
	}
	else
	{
		//TYlog << "Shader: " << pComputePath << " successfully LINKED" << TYlogbreak;
	}

	SetupUniforms();
}

void Shader::SetupUniforms()
{
	// Setup Uniform Map
	TYint max_length;
	glGetProgramiv(Program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_length);
	TYint num_uniforms;

	TYchar* pname = new TYchar[max_length];

	glGetProgramiv(Program, GL_ACTIVE_UNIFORMS, &num_uniforms);

	for (TYint i = 0; i < num_uniforms; i++)
	{
		GLsizei written;
		TYint size;
		GLenum type;

		glGetActiveUniform(Program, i, max_length, &written, &size, &type, pname);

		TYchar* pname1 = new TYchar[max_length];

		std::strcpy(pname1, pname);

		if (size > 1)
		{
			pname1[written - 3] = '\0';
		}

		TYint loc = glGetUniformLocation(Program, pname1);

		Uniforms.insert(std::pair<TYchar*, TYint>(pname1, loc));
	}

	delete[] pname;
}

void Shader::Use()
{
	glUseProgram(this->Program);
}

void Shader::setBool(TYint uniformLoc, TYbool value)
{
	glUniform1i(uniformLoc, (TYint)value);
}

void Shader::setInt(TYint uniformLoc, TYint value)
{
	glUniform1i(uniformLoc, value);
}

void Shader::setFloat(TYint uniformLoc, TYfloat value)
{
	glUniform1f(uniformLoc, value);
}

void Shader::set3FloatArray(TYint uniformLoc, const TYfloat* value, TYint count)
{
	glUniform3fv(uniformLoc, count, &value[0]);
}
void Shader::set4FloatArray(TYint uniformLoc, const TYfloat* value, TYint count)
{
	glUniform4fv(uniformLoc, count, &value[0]);
}

void Shader::setIntArray(TYint uniformLoc, TYvectorI& value, TYint count)
{
	glUniform1iv(uniformLoc, count, &value[0]);
}

void Shader::set1FloatArray(TYint uniformLoc, TYvectorF& value, TYint count)
{
	glUniform1fv(uniformLoc, count, &value[0]);
}
void Shader::set2FloatArray(TYint uniformLoc, TYvector<TYvec2>& value, TYint count)
{
	glUniform2fv(uniformLoc, count, &value[0].x);
}
void Shader::set3FloatArray(TYint uniformLoc, TYvector3& value, TYint count)
{
	glUniform3fv(uniformLoc, count, &value[0].x);
}
void Shader::set4FloatArray(TYint uniformLoc, TYvector<TYvec4>& value, TYint count)
{
	glUniform4fv(uniformLoc, count, &value[0].x);
}

void Shader::setVec2(TYint uniformLoc, const TYvec2 &value)
{
	glUniform2fv(uniformLoc, 1, &value[0]);
}
void Shader::setVec2(TYint uniformLoc, TYfloat x, TYfloat y)
{
	glUniform2f(uniformLoc, x, y);
}

void Shader::setVec3(TYint uniformLoc, const TYvec &value)
{
	glUniform3fv(uniformLoc, 1, &value[0]);
}
void Shader::setVec3(TYint uniformLoc, TYfloat x, TYfloat y, TYfloat z)
{
	glUniform3f(uniformLoc, x, y, z);
}

void Shader::setVec4(TYint uniformLoc, const TYvec4 &value)
{
	glUniform4fv(uniformLoc, 1, &value[0]);
}
void Shader::setVec4(TYint uniformLoc, TYfloat x, TYfloat y, TYfloat z, TYfloat w)
{
	glUniform4f(uniformLoc, x, y, z, w);
}

void Shader::setMat3(TYint uniformLoc, const TYmat3 &mat)
{
	glUniformMatrix3fv(uniformLoc, 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat4(TYint uniformLoc, const TYmat &mat)
{
	glUniformMatrix4fv(uniformLoc, 1, GL_FALSE, &mat[0][0]);
}
