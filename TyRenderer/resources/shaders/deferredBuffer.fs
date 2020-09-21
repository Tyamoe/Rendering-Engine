#version 460 core

layout (location = 0) out vec4 positionTexture;
layout (location = 1) out vec4 normalTexture;
layout (location = 2) out vec4 colorTexture;

in vec3 fragPos;
in vec4 Normal;
in vec2 TexCoord;

//uniform sampler2D meshTexture;
uniform vec4 meshColor;

void main()
{
	positionTexture = vec4(fragPos, 1.0f);
	normalTexture = vec4(Normal.xyz, 1.0f);
	colorTexture = meshColor;
}