#include "stdafx.h"

#include "Types.h"

#ifndef TYAMOE3D

static TYstring path = "resources/shaders/";

#else

#include "Tyamoe3DHelper.h"
#include EngineInc(stdafx.h)

static TYstring path = "TyRenderer/TyRenderer/resources/shaders/";

#endif // TYAMOE3D

#include "Shader.h"

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
////////////////////////////////////////////////////////////////////////////////
TYvoid ErrorSet(...)
{
	std::cout << "ErrorSet()" << std::endl;
}

TYvoid setBool(TYint uniformLoc, TYbool value)
{
	glUniform1i(uniformLoc, (TYint)value);
}

TYvoid setInt(TYint uniformLoc, TYint value)
{
	glUniform1i(uniformLoc, value);
}

TYvoid setFloat(TYint uniformLoc, TYfloat value)
{
	glUniform1f(uniformLoc, value);
}

TYvoid set3FloatArray(TYint uniformLoc, const TYfloat* value, TYint count)
{
	glUniform3fv(uniformLoc, count, &value[0]);
}
TYvoid set4FloatArray(TYint uniformLoc, const TYfloat* value, TYint count)
{
	glUniform4fv(uniformLoc, count, &value[0]);
}

TYvoid setIntArray(TYint uniformLoc, TYvectorI& value, TYint count)
{
	glUniform1iv(uniformLoc, count, &value[0]);
}

TYvoid set1FloatArray(TYint uniformLoc, TYvectorF& value, TYint count)
{
	glUniform1fv(uniformLoc, count, &value[0]);
}
TYvoid set2FloatArray(TYint uniformLoc, TYvector<TYvec2>& value, TYint count)
{
	glUniform2fv(uniformLoc, count, &value[0].x);
}
TYvoid set3FloatArray(TYint uniformLoc, TYvector3& value, TYint count)
{
	glUniform3fv(uniformLoc, count, &value[0].x);
}
TYvoid set4FloatArray(TYint uniformLoc, TYvector<TYvec4>& value, TYint count)
{
	glUniform4fv(uniformLoc, count, &value[0].x);
}

TYvoid setVec2(TYint uniformLoc, TYvec2 value)
{
	glUniform2fv(uniformLoc, 1, &value[0]);
}

TYvoid setVec3(TYint uniformLoc, TYvec value)
{
	glUniform3fv(uniformLoc, 1, &value[0]);
}

TYvoid setVec4(TYint uniformLoc, TYvec4 value)
{
	glUniform4fv(uniformLoc, 1, &value[0]);
}

TYvoid setMat3(TYint uniformLoc, TYmat3 mat)
{
	glUniformMatrix3fv(uniformLoc, 1, GL_FALSE, &mat[0][0]);
}

TYvoid setMat4(TYint uniformLoc, TYmat mat)
{
	glUniformMatrix4fv(uniformLoc, 1, GL_FALSE, &mat[0][0]);
}

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
////////////////////////////////////////////////////////////////////////////////

typedef TYvoid(*GenericSetter)(TYint);

// https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glGetActiveUniform.xhtml
TYumap<TYint, GenericSetter> TypeFuncs =
{
	{-1, (GenericSetter)ErrorSet},
	{GL_BOOL, (GenericSetter)setBool},
	{GL_INT, (GenericSetter)setInt},
	{GL_FLOAT, (GenericSetter)setFloat},

	{GL_FLOAT_VEC2, (GenericSetter)setVec2},
	{GL_FLOAT_VEC3, (GenericSetter)setVec3},
	{GL_FLOAT_VEC4, (GenericSetter)setVec4},

	{GL_FLOAT_MAT3, (GenericSetter)setMat3},
	{GL_FLOAT_MAT4, (GenericSetter)setMat4},

	{GL_INT + 1, (GenericSetter)setIntArray},
	{GL_FLOAT + 1, (GenericSetter)set1FloatArray},

	{GL_SAMPLER_1D, (GenericSetter)setInt},
	{GL_SAMPLER_2D, (GenericSetter)setInt},
	{GL_SAMPLER_3D, (GenericSetter)setInt},
	{GL_SAMPLER_CUBE, (GenericSetter)setInt},

	{GL_SAMPLER_1D_SHADOW, (GenericSetter)setInt},
	{GL_SAMPLER_2D_SHADOW, (GenericSetter)setInt},

	{GL_IMAGE_1D, (GenericSetter)setInt},
	{GL_IMAGE_2D, (GenericSetter)setInt},
	{GL_IMAGE_3D, (GenericSetter)setInt},
	{GL_IMAGE_CUBE, (GenericSetter)setInt},
};

typedef TYvoid(*IntSetter)(TYint, TYint);
typedef TYvoid(*BoolSetter)(TYint, TYbool);
typedef TYvoid(*FloatSetter)(TYint, TYfloat);

typedef TYvoid(*Vec2Setter)(TYint, TYvec2);
typedef TYvoid(*Vec3Setter)(TYint, TYvec);
typedef TYvoid(*Vec4Setter)(TYint, TYvec4);

typedef TYvoid(*Mat3Setter)(TYint, TYmat3);
typedef TYvoid(*Mat4Setter)(TYint, TYmat);

typedef TYvoid(*IntArrSetter)(TYint, TYvectorI&, TYint);
typedef TYvoid(*FloatArrSetter)(TYint, TYvectorF&, TYint);

TYvoid UniformSetter::operator()(TYint value)
{
	((IntSetter)TypeFuncs[type])(loc, value);
}

TYvoid UniformSetter::operator()(TYbool value)
{
	((BoolSetter)TypeFuncs[type])(loc, value);
}

TYvoid UniformSetter::operator()(TYfloat value)
{
	((FloatSetter)TypeFuncs[type])(loc, value);
}

TYvoid UniformSetter::operator()(TYvec2 value)
{
	((Vec2Setter)TypeFuncs[type])(loc, value);
}

TYvoid UniformSetter::operator()(TYvec value)
{
	((Vec3Setter)TypeFuncs[type])(loc, value);
}

TYvoid UniformSetter::operator()(TYvec4 value)
{
	((Vec4Setter)TypeFuncs[type])(loc, value);
}

TYvoid UniformSetter::operator()(TYmat3 value)
{
	((Mat3Setter)TypeFuncs[type])(loc, value);
}

TYvoid UniformSetter::operator()(TYmat value)
{
	((Mat4Setter)TypeFuncs[type])(loc, value);
}

TYvoid UniformSetter::operator()(TYvectorI& value, TYint count)
{
	((IntArrSetter)TypeFuncs[type])(loc, value, count);
}

TYvoid UniformSetter::operator()(TYvectorF& value, TYint count)
{
	((FloatArrSetter)TypeFuncs[type])(loc, value, count);
}

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
////////////////////////////////////////////////////////////////////////////////

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
	SetupUniforms();
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

	//TYlog << TYlogbreak << pComputePath << TYlogbreak;
	SetupUniforms();
}

TYvoid Shader::SetupUniforms()
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
			type += 1;
		}

		TYint loc = glGetUniformLocation(Program, pname1);

		//Uniforms.insert(std::pair<TYchar*, TYint>(pname1, loc));
		/*if (glGetString(GL_VERSION) == 0)
		{
			GLenum err = glGetError();
			TYlog << glewGetErrorString(err) << "\n";
		}
		TYlog << pname << " is " << (glGetString(GL_INT)) << " = " << (TYint)type<< "\n";*/

		UniformSetter setter;
		setter.loc = loc;
		setter.type = type;
		Uniforms.insert({ pname1, setter });

		if (TypeFuncs.find((TYint)type) == TypeFuncs.end())
		{
			std::cout << "Unsupported type: " << type << std::endl;
		}
	}

	delete[] pname;
}

TYvoid Shader::Use()
{
	glUseProgram(this->Program);
}