#version 460 core

layout(location = 0) in vec3 vertPosition;
layout(location = 1) in vec3 vertNormal;
layout(location = 2) in vec2 vertTexCoord;
layout(location = 3) in ivec4 boneIds;
layout(location = 4) in vec4 boneWeights;

out vec3 fragPos;
out vec4 Normal;
out vec2 TexCoord;

const int MAX_BONES = 100;

uniform mat4 MVP;
uniform mat4 Model;
//uniform mat4 InvTrModel;

uniform mat4 SkeletonTransform[MAX_BONES];

void main()
{
	/*TexCoord = vertTexCoord;

	vec3 norm = normalize(vertNormal);
	fragPos = vec3(Model * vec4(vertPosition, 1.0f));
	Normal = InvTrModel * vec4(norm, 1.0f); 
	Normal = normalize(Normal);

	gl_Position = MVP * vec4(vertPosition, 1.0f);*/
	
	mat4 boneTransform  =  mat4(0.0f);
		
	boneTransform += SkeletonTransform[boneIds[0]] * boneWeights.x;
	boneTransform += SkeletonTransform[boneIds[1]] * boneWeights.y;
	boneTransform += SkeletonTransform[boneIds[2]] * boneWeights.z;
	boneTransform += SkeletonTransform[boneIds[3]] * boneWeights.w;
		
	vec4 pos = boneTransform * vec4(vertPosition, 1.0f);
		
	gl_Position = MVP * pos;
		
	fragPos = vec3(Model * pos);
		
	TexCoord = vertTexCoord;
		
	Normal = transpose(inverse(Model * boneTransform)) * vec4(vertNormal, 1.0f);
	Normal = normalize(Normal);
}