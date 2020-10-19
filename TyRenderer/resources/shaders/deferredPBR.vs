#version 460 core

layout (location = 0) in vec3 vertPosition;
layout (location = 1) in vec2 vertTexCoord;

out vec2 TexCoord;

void main()
{
	TexCoord = vertTexCoord;
	gl_Position = vec4(vertPosition, 1.0f);
}