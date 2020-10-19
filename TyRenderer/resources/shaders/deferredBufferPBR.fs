#version 460 core

layout (location = 0) out vec4 positionTexture;
layout (location = 1) out vec4 normalTexture;
layout (location = 2) out vec4 colorTexture;
layout (location = 3) out uvec4 extraTexture;

in vec3 fragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D aoMap;

vec3 getNormalFromMap();

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
	normalTexture = vec4(getNormalFromMap(), 1.0f);
	colorTexture = texture(albedoMap, TexCoord);

	float met = texture(metallicMap, TexCoord).r;
	float roughness = texture(roughnessMap, TexCoord).r;
	float ao = texture(aoMap, TexCoord).r;

	/////////////////////////////////////////////////////
	unsigned int pack = packVec(vec4(met, roughness, ao, 1.0f));

	extraTexture = uvec4(pack, 1, 0, 1);
}

vec3 getNormalFromMap()
{
    vec3 tangentNormal = texture(normalMap, TexCoord).xyz * 2.0f - 1.0f;

    vec3 Q1  = dFdx(fragPos);
    vec3 Q2  = dFdy(fragPos);
    vec2 st1 = dFdx(TexCoord);
    vec2 st2 = dFdy(TexCoord);

    vec3 N   = normalize(Normal);
    vec3 T  = normalize(Q1 * st2.t - Q2 * st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}