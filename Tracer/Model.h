#pragma once

#include "Types.h"

typedef class Model
{
public:
	Model(std::string filePath); // object
	Model(); // sphere
	Model(bool); // square
	~Model();

	glm::mat4 GetMatrix();

	GLuint VAO;
	GLuint VBO;
	GLuint IBO;
	int Count;

	glm::vec3 Color = glm::vec3(1, 0, 0);

	std::vector<GLfloat> Vertices;
	std::vector<GLfloat> Normals;
	std::vector<unsigned> Indices;

	glm::vec3 Position = glm::vec3(0);
	glm::vec3 Scale = glm::vec3(1.f, 1.f, 1.f);

	float angle = 0.0f;
	bool sphere = false;
private:

	void SetupBuffers();

}Model, *ModelPtr;