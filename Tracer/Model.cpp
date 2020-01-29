#include "stdafx.h"

#include <fstream>

#include "Model.h"

std::vector<glm::vec3> SphereVertices;
std::vector<unsigned>  SphereIndices;
std::vector<glm::vec3> SphereNormals;

// Sphere
Model::Model()
{
	glm::vec3 vertexPosition, vertexNormal;
	GLfloat xy;
	GLfloat radius = 0.1f;
	unsigned stackCount = 20;
	unsigned sectorCount = 25;

	GLfloat nx, ny, nz, lengthInv = 1.0f / radius;

	GLfloat stackStep = glm::pi<GLfloat>() / stackCount;
	GLfloat sectorStep = 2 * glm::pi<GLfloat>() / sectorCount;
	GLfloat stackAngle, sectorAngle;

	for (int i = 0; i <= stackCount; i++)
	{
		stackAngle = glm::pi<GLfloat>() / 2 - i * stackStep;
		xy = radius * cosf(stackAngle);
		vertexPosition.z = radius * sinf(stackAngle);

		for (int j = 0; j <= sectorCount; j++)
		{
			sectorAngle = j * sectorStep;

			vertexPosition.x = xy * cosf(sectorAngle);
			vertexPosition.y = xy * sinf(sectorAngle);

			Vertices.push_back(vertexPosition.x);
			Vertices.push_back(vertexPosition.y);
			Vertices.push_back(vertexPosition.z);

			nx = vertexPosition.x * lengthInv;
			ny = vertexPosition.y * lengthInv;
			nz = vertexPosition.z * lengthInv;

			Vertices.push_back(nx);
			Vertices.push_back(ny);
			Vertices.push_back(nz);
		}
	}

	int current, next;
	for (int i = 0; i < stackCount; i++)
	{
		current = i * (sectorCount + 1);   // Current stack beginning
		next = current + sectorCount + 1;  // Next stack beginning

		for (int j = 0; j < sectorCount; j++, current++, next++)
		{
			if (i != 0)
			{
				Indices.push_back(current);
				Indices.push_back(next);
				Indices.push_back(current + 1);
			}

			if (i != (stackCount - 1))
			{
				Indices.push_back(current + 1);
				Indices.push_back(next);
				Indices.push_back(next + 1);
			}
		}
	}

	for (int i = 0; i < Indices.size(); i += 3)
	{
		GLfloat x1, y1, z1;
		GLfloat i1 = Indices[i];
		GLfloat i2 = Indices[i + 1];
		GLfloat i3 = Indices[i + 2];

		x1 = (Vertices[i1 * 6 + 0] + Vertices[i2 * 6 + 0] + Vertices[i3 * 6 + 0]) / 3.0f;
		y1 = (Vertices[i1 * 6 + 1] + Vertices[i2 * 6 + 1] + Vertices[i3 * 6 + 1]) / 3.0f;
		z1 = (Vertices[i1 * 6 + 2] + Vertices[i2 * 6 + 2] + Vertices[i3 * 6 + 2]) / 3.0f;

		nx = (Vertices[i1 * 6 + 3] + Vertices[i2 * 6 + 3] + Vertices[i3 * 6 + 3]) / 3.0f;
		ny = (Vertices[i1 * 6 + 4] + Vertices[i2 * 6 + 4] + Vertices[i3 * 6 + 4]) / 3.0f;
		nz = (Vertices[i1 * 6 + 5] + Vertices[i2 * 6 + 5] + Vertices[i3 * 6 + 5]) / 3.0f;

		Normals.push_back(x1);
		Normals.push_back(y1);
		Normals.push_back(z1);

		glm::vec3 norm1 = glm::vec3(nx, ny, nz);
		norm1 = glm::normalize(norm1);

		Normals.push_back(x1 + norm1[0] * 0.02f);
		Normals.push_back(y1 + norm1[1] * 0.02f);
		Normals.push_back(z1 + norm1[2] * 0.02f);
	}

	SetupBuffers();
}

// OBJ Loader
Model::Model(std::string filePath)
{
	std::vector<GLfloat> vertices;
	std::vector<glm::vec3> vertexSums;
	std::vector<GLfloat> faceNormals;

	GLfloat minX = INFINITY;
	GLfloat maxX = -INFINITY;
	GLfloat minY = INFINITY;
	GLfloat maxY = -INFINITY;
	GLfloat minZ = INFINITY;
	GLfloat maxZ = -INFINITY;

	GLfloat MaxDistance = 0.0f;
	GLfloat MaxMagnitude = 0.0f;
	bool ObjectNormals = false;

	std::ifstream file(filePath);

	while (!file.eof())
	{
		std::string input;
		std::getline(file, input);

		std::vector<std::string> line;

		if (input[0] != 'v' && input[0] != 'f')
			continue;

		char* poop = _strdup(input.c_str());
		char* tok = strtok(poop, " \t");
		while (tok)
		{
			if (tok) line.push_back(tok);
			tok = strtok(NULL, " \t");
		}

		if (line[0] == "v")  // Vertex position
		{
			glm::vec3 vertex = { stof(line[1]), stof(line[2]), stof(line[3]) };
			GLfloat distance = glm::distance(vertex, glm::vec3(0, 0, 0));
			if (distance > MaxDistance)  // Check if new max distance
			{
				MaxDistance = distance;
				MaxMagnitude = glm::length(vertex);
			}

			if (vertex.x > maxX)
				maxX = vertex.x;
			if (vertex.x < minX)
				minX = vertex.x;
			if (vertex.y > maxY)
				maxY = vertex.y;
			if (vertex.y < minY)
				minY = vertex.y;
			if (vertex.z > maxZ)
				maxZ = vertex.z;
			if (vertex.z < minZ)
				minZ = vertex.z;

			vertices.push_back(vertex.x);
			vertices.push_back(vertex.y);
			vertices.push_back(vertex.z);
			vertexSums.push_back(glm::vec3(0, 0, 0));
		}
		else if (line[0] == "f")  // Face
		{
			int lineNum = 1;  // Stores which input to check
			for (int i = 0; i < line[lineNum].length(); i++)  // Check for input containing '/'
			{
				if (line[lineNum][i] == '/')  // Remove excess information
				{
					line[lineNum] = line[lineNum].substr(0, i);
					if (lineNum == 3)  // End when completed all three inputs
						break;
					lineNum++;
					i = 0;
				}
			}

			Indices.push_back(stoi(line[1]) - 1);
			Indices.push_back(stoi(line[2]) - 1);
			Indices.push_back(stoi(line[3]) - 1);

			if (line.size() > 4 && line[4] != "")
			{
				for (int i = 4; i < line.size(); i++)
				{
					Indices.push_back(stoi(line[1]) - 1);
					Indices.push_back(stoi(line[i - 1]) - 1);
					Indices.push_back(stoi(line[i]) - 1);
				}
			}
		}
	}

	if (!ObjectNormals)
	{
		glm::vec3 vertex1, vertex2, vertex3, vector1, vector2, crossProduct;

		GLfloat w = std::abs(maxX - minX);
		GLfloat h = std::abs(maxY - minY);
		GLfloat d = std::abs(maxZ - minZ);

		GLfloat dx = maxX + minX;
		GLfloat dy = maxY + minY;
		GLfloat dz = maxZ + minZ;

		GLfloat sx = (dx / 2.0);
		GLfloat sy = (dy / 2.0);
		GLfloat sz = (dz / 2.0);

		for (int i = 0; i < Indices.size(); i += 3)
		{
			GLfloat oldLow = 0.0f;
			GLfloat oldHigh = MaxDistance;
			GLfloat newLow = -1.0f;
			GLfloat newHigh = 1.0f;

			unsigned index1 = Indices[i] * 3;
			unsigned index2 = Indices[i + 1] * 3;
			unsigned index3 = Indices[i + 2] * 3;

			vertex1 = glm::vec3(vertices[index1], vertices[index1 + 1], vertices[index1 + 2]);
			vertex2 = glm::vec3(vertices[index2], vertices[index2 + 1], vertices[index2 + 2]);
			vertex3 = glm::vec3(vertices[index3], vertices[index3 + 1], vertices[index3 + 2]);

			GLfloat x = (vertices[index1] + vertices[index2] + vertices[index3]) / 3.0;
			GLfloat y = (vertices[index1 + 1] + vertices[index2 + 1] + vertices[index3 + 1]) / 3.0;
			GLfloat z = (vertices[index1 + 2] + vertices[index2 + 2] + vertices[index3 + 2]) / 3.0;

			x -= sx;
			y -= sy;
			z -= sz;

			x = ((x - oldLow) / (oldHigh - oldLow)) * (newHigh - newLow + newLow);
			y = ((y - oldLow) / (oldHigh - oldLow)) * (newHigh - newLow + newLow);
			z = ((z - oldLow) / (oldHigh - oldLow)) * (newHigh - newLow + newLow);

			vector1 = vertex1 - vertex2;
			vector2 = vertex1 - vertex3;
			crossProduct = cross(vector1, vector2);
			normalize(crossProduct);

			Normals.push_back(x);
			Normals.push_back(y);
			Normals.push_back(z);

			Normals.push_back(x + crossProduct[0] * 0.03f);
			Normals.push_back(y + crossProduct[1] * 0.03f);
			Normals.push_back(z + crossProduct[2] * 0.03f);

			index1 = index1 / 3;
			index2 = index2 / 3;
			index3 = index3 / 3;

			vertexSums[index1] += crossProduct;
			vertexSums[index2] += crossProduct;
			vertexSums[index3] += crossProduct;
		}
		// Vertex Normals

		for (int i = 0; i < vertices.size(); i += 3)
		{

			GLfloat oldLow = 0.0f;
			GLfloat oldHigh = MaxDistance;
			GLfloat newLow = -1.0f;
			GLfloat newHigh = 1.0f;

			GLfloat x = vertices[i + 0];
			GLfloat y = vertices[i + 1];
			GLfloat z = vertices[i + 2];

			x -= sx;
			y -= sy;
			z -= sz;

			x = ((x - oldLow) / (oldHigh - oldLow)) * (newHigh - newLow + newLow);
			y = ((y - oldLow) / (oldHigh - oldLow)) * (newHigh - newLow + newLow);
			z = ((z - oldLow) / (oldHigh - oldLow)) * (newHigh - newLow + newLow);

			Vertices.push_back(x);
			Vertices.push_back(y);
			Vertices.push_back(z);

			glm::vec3 nn = glm::normalize(vertexSums[i / 3]);

			Vertices.push_back(x + (nn[0] * 0.02));
			Vertices.push_back(y + (nn[1] * 0.02));
			Vertices.push_back(z + (nn[2] * 0.02));
		}
	}

	SetupBuffers();
}

Model::Model(bool)
{
	GLfloat boxVertices[] =
	{ // X, Y, Z           U, V
				// Top
			-0.5, 0.5, -0.5, 0, 0,													0,
			-0.5, 0.5, 0.5, 0, 1,													0,
			0.5, 0.5, 0.5, 1, 1,													0,
			0.5, 0.5, -0.5, 1, 0,													0,

			// Left																	0,
			-0.5, 0.5, 0.5, 1, 0,													0,
			-0.5, -0.5, 0.5, 1, 1,													0,
			-0.5, -0.5, -0.5, 0, 1,													0,
			-0.5, 0.5, -0.5, 0, 0,													0,

			// Right																0,
			0.5, 0.5, 0.5, 0, 0,													0,
			0.5, -0.5, 0.5, 0, 1,													0,
			0.5, -0.5, -0.5, 1, 1,													0,
			0.5, 0.5, -0.5, 1, 0,													0,

			// Front																0,
			0.5, 0.5, 0.5, 1, 0,													0,
			0.5, -0.5, 0.5, 1, 1,													0,
			-0.5, -0.5, 0.5, 0, 1,													0,
			-0.5, 0.5, 0.5, 0, 0,													0,

			// Back																	0,
			0.5, 0.5, -0.5, 0, 0,													0,
			0.5, -0.5, -0.5, 0, 1,													0,
			-0.5, -0.5, -0.5, 1, 1,													0,
			-0.5, 0.5, -0.5, 1, 0,													0,

			// Bottom																0,
			-0.5, -0.5, -0.5, 0, 1,													0,
			-0.5, -0.5, 0.5, 0, 0,													0,
			0.5, -0.5, 0.5, 1, 0,													0,
			0.5, -0.5, -0.5, 1, 1,													0
	};

	unsigned boxIndices[] =
	{
		// Top
		0, 1, 2,
		0, 2, 3,

		// Left
		5, 4, 6,
		6, 4, 7,

		// Right
		8, 9, 10,
		8, 10, 11,

		// Front
		13, 12, 14,
		15, 14, 12,

		// Back
		16, 17, 18,
		16, 18, 19,

		// Bottom
		21, 20, 22,
		22, 20, 23
	};

	for (auto& a : boxVertices)
		Vertices.push_back(a);
	for (auto& FUCK : boxIndices)
		Indices.push_back(FUCK);
	SetupBuffers(); // nice
}

void Model::SetupBuffers()
{
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glGenBuffers(1, &IBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(GLfloat), &Vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.size() * sizeof(GLuint), &Indices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
}

glm::mat4 Model::GetMatrix()
{
	glm::mat4 model = glm::mat4(1);
	model = glm::translate(model, Position);
	model = glm::scale(model, Scale);

	if (sphere)
	{
		angle += 0.1f;

		float theta = glm::radians(angle);

		float x = 3 * glm::sin(theta);
		float z = 3 * glm::cos(theta);

		Position.x = x;
		Position.z = z;
	}

	return model;
}

Model::~Model()
{
	GLint max_vtx_attrib = 0;
	GLuint buffer_object;

	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max_vtx_attrib);
	glBindVertexArray(VAO);

	for (int i = 0; i < max_vtx_attrib; ++i)
	{
		glGetVertexAttribIuiv(i, GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING, &buffer_object);
		if (buffer_object > 0)
		{
			glDeleteBuffers(1, &buffer_object);
		}
	}

	glGetVertexArrayiv(VAO, GL_ELEMENT_ARRAY_BUFFER_BINDING, reinterpret_cast<GLint*>(&buffer_object));

	if (buffer_object > 0)
	{
		glDeleteBuffers(1, &buffer_object);
	}

	glDeleteVertexArrays(1, &VAO);
}