#version 460 core

in vec3 fragPos;
in vec4 Normal;

out vec4 fcolor;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;
uniform vec4 oColor;

void main()
{
	// ambient
	float ambientStrength = 0.1f;
	vec3 ambient = ambientStrength * lightColor;
	
	// diffuse 
	vec3 norm = normalize(Normal.xyz);
	vec3 lightDir = normalize(lightPos - fragPos);
	float diff = max(dot(norm, lightDir), 0.0f);
	vec3 diffuse = diff * lightColor;
	
	// specular
	float specularStrength = 0.5f;
	vec3 viewDir = normalize(viewPos - fragPos);
	vec3 reflectDir = reflect(-lightDir, norm);  
	float spec = pow(max(dot(viewDir, reflectDir), 0.0f), 32.0f);
	vec3 specular = specularStrength * spec * lightColor;  
		
	vec3 result = (ambient + diffuse + specular) * oColor.xyz;

	fcolor = vec4(result, 1.0f);
}