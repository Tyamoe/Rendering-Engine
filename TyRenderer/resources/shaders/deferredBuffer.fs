#version 460 core

layout (location = 0) out vec4 positionTexture;
layout (location = 1) out vec4 normalTexture;
layout (location = 2) out vec4 colorTexture;
layout (location = 3) out uvec4 extraTexture;

in vec3 fragPos;
in vec4 Normal;
in vec2 TexCoord;

//uniform sampler2D meshTexture;
uniform vec4 meshColor;
uniform sampler2D diffuseTexture;

unsigned int packVec(vec4 value)
{
	unsigned int pack = 0;

	pack |= unsigned int((value.x * 255.0f)) << 24;
	pack |= unsigned int((value.y * 255.0f)) << 16;
	pack |= unsigned int((value.z * 255.0f)) << 8;
	pack |= unsigned int((value.w * 255.0f)) << 0;

	return pack;
}

void main()
{
	positionTexture = vec4(fragPos, 1.0f);
	normalTexture = vec4(Normal.xyz, 1.0f);

  	vec4 diff = texture(diffuseTexture, TexCoord);
	colorTexture = meshColor * diff;

	/////////////////////////////////////////////////////
	unsigned int mColor = packVec(meshColor);
	extraTexture = uvec4(mColor, 0, 1, 1);
	/////////////////////////////////////////////////////
}