#version 460 core
layout (location = 0) in vec2 vertPosition;
layout (location = 1) in vec2 vertTexCoord;
out vec2 TexCoord;

uniform mat4 Proj;
uniform mat4 View;
uniform mat4 Model;

uniform float repeat;

void main()
{
	gl_Position = Proj * View * Model * vec4(vertPosition, 0.0f, 1.0f);
	TexCoord = vertTexCoord * repeat;
}