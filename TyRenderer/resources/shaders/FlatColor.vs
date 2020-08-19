#version 460 core

layout (location = 0) in vec3 vertPosition;
layout (location = 0) in vec3 vertNormal;
layout (location = 0) in vec2 vertTexCoord;

uniform mat4 MVP;

void main()
{
	gl_Position = MVP * vec4(vertPosition, 1.0f);
}