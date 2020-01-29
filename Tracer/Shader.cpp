#include "stdafx.h"
#include "Shader.h"

static std::string path = "resources/shaders/";

Shader::~Shader()
{
	glDeleteProgram(Program);
}

Shader::Shader(std::string vertexPath, std::string fragmentPath)
{
	vertexPath = path + vertexPath;
	fragmentPath = path + fragmentPath;

	std::string vertexCode;
	std::string fragmentCode;

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

	const GLchar* vShaderCode = vertexCode.c_str();
	const GLchar* fShaderCode = fragmentCode.c_str();

	GLuint vertex, fragment;
	GLint success;
	GLchar infoLog[512];

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
		GLint max_length;
		glGetProgramiv(Program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_length);
		GLint num_uniforms;

		GLchar* pname = new GLchar[max_length];

		glGetProgramiv(Program, GL_ACTIVE_UNIFORMS, &num_uniforms);

		for (GLint i = 0; i < num_uniforms; ++i) 
		{
			GLsizei written;
			GLint size;
			GLenum type;
			
			glGetActiveUniform(Program, i, max_length, &written, &size, &type, pname);

			GLchar* pname1 = new GLchar[max_length];

			std::strcpy(pname1, pname);

			if (size > 1)
			{
				pname1[written - 3] = '\0';
			}

			GLint loc = glGetUniformLocation(Program, pname1);

			Uniforms.insert(std::pair<GLchar*, GLint>(pname1, loc));
		}

		delete[] pname;
	}
}

void Shader::Use()
{
	glUseProgram(this->Program);
}

void Shader::setBool(GLint uniformLoc, bool value)
{
	glUniform1i(uniformLoc, (int)value);
}

void Shader::setInt(GLint uniformLoc, int value)
{
	glUniform1i(uniformLoc, value);
}

void Shader::setFloat(GLint uniformLoc, float value)
{
	glUniform1f(uniformLoc, value);
}

void Shader::set3FloatArray(GLint uniformLoc, const float* value, int count)
{
	glUniform3fv(uniformLoc, count, &value[0]);
}
void Shader::set4FloatArray(GLint uniformLoc, const float* value, int count)
{
	glUniform4fv(uniformLoc, count, &value[0]);
}

void Shader::setIntArray(GLint uniformLoc, std::vector<int>& value, int count)
{
	glUniform1iv(uniformLoc, count, &value[0]);
}

void Shader::set1FloatArray(GLint uniformLoc, std::vector<float>& value, int count)
{
	glUniform1fv(uniformLoc, count, &value[0]);
}
void Shader::set2FloatArray(GLint uniformLoc, std::vector<glm::vec2>& value, int count)
{
	glUniform2fv(uniformLoc, count, &value[0].x);
}
void Shader::set3FloatArray(GLint uniformLoc, std::vector<glm::vec3>& value, int count)
{
	glUniform3fv(uniformLoc, count, &value[0].x);
}
void Shader::set4FloatArray(GLint uniformLoc, std::vector<glm::vec4>& value, int count)
{
	glUniform4fv(uniformLoc, count, &value[0].x);
}

void Shader::setVec2(GLint uniformLoc, const glm::vec2 &value)
{
	glUniform2fv(uniformLoc, 1, &value[0]);
}
void Shader::setVec2(GLint uniformLoc, float x, float y)
{
	glUniform2f(uniformLoc, x, y);
}

void Shader::setVec3(GLint uniformLoc, const glm::vec3 &value)
{
	glUniform3fv(uniformLoc, 1, &value[0]);
}
void Shader::setVec3(GLint uniformLoc, float x, float y, float z)
{
	glUniform3f(uniformLoc, x, y, z);
}

void Shader::setVec4(GLint uniformLoc, const glm::vec4 &value)
{
	glUniform4fv(uniformLoc, 1, &value[0]);
}
void Shader::setVec4(GLint uniformLoc, float x, float y, float z, float w)
{
	glUniform4f(uniformLoc, x, y, z, w);
}

void Shader::setMat3(GLint uniformLoc, const glm::mat3 &mat)
{
	glUniformMatrix3fv(uniformLoc, 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat4(GLint uniformLoc, const glm::mat4 &mat)
{
	glUniformMatrix4fv(uniformLoc, 1, GL_FALSE, &mat[0][0]);
}
