#version 460 core

layout(location = 0) in vec3 vertPosition;
layout(location = 1) in vec3 vertNormal;
layout(location = 2) in vec2 vertTexCoord;

out vec3 fragPos;
out vec3 Normal;
out vec2 TexCoord;

uniform mat4 MVP;
uniform mat4 Model;

void main()
{
	TexCoord = vertTexCoord;

	fragPos = vec3(Model * vec4(vertPosition, 1.0f));

	Normal = mat3(Model) * vertNormal;   

	gl_Position = MVP * vec4(vertPosition, 1.0f);
}