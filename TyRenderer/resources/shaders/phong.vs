#version 460 core

layout (location = 0) in vec3 vertPosition;
layout (location = 1) in vec3 vertNormal;

out vec3 fragPos;
out vec4 Normal;

uniform mat4 MVP;
uniform mat4 Model;
uniform mat4 InvTrModel;

void main()
{
	vec3 norm = vertNormal - vertPosition;
	fragPos = vec3(Model * vec4(vertPosition, 1.0f));
	Normal = InvTrModel * vec4(normalize(norm), 1.0f); 

	gl_Position = MVP * vec4(vertPosition, 1.0f);
}