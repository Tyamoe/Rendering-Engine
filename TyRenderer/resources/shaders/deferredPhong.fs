#version 460 core

in vec2 TexCoord;

out vec4 color;

const int L_POINT  =1;
const int L_DIR    =2;
const int L_SPOT   =3;
const int L_MAX_NUM=16;

struct Light
{
	int type;
	vec3 ambi;
	vec3 diff;
	vec3 spec;			 
	vec3 pos;
	float c0;
	float c1;
	float c2;
	vec3 dir;
	float innerTheta;
	float outerTheta;
	float spotIntensity;
};

uniform sampler2D positionTexture;
uniform sampler2D normalTexture;
uniform sampler2D colorTexture;

uniform int num_active_lights;
uniform vec3 viewPos;
uniform vec3 ambient;
uniform vec3 fog;
uniform float ZNear;
uniform float ZFar;
uniform Light lights[L_MAX_NUM];

void main()
{
	vec3 defPos = texture(positionTexture, TexCoord).xyz;
	vec3 defNorm = texture(normalTexture, TexCoord).xyz;
	vec3 defColor = texture(colorTexture, TexCoord).xyz;

	vec3 viewDir = normalize(viewPos - defPos);
	vec3 norm = normalize(defNorm);

	float ambientStrength = 0.1f;
	float specularStrength = 0.5f;

	vec3 result = defColor + ambientStrength * ambient;

	for(int i = 0; i < num_active_lights; i++)
	{
		//int index = i;//i * 7;
	    vec3 Pos  = lights[i].pos;//vec3(lights[index + 0]);
	    vec3 Amb  = lights[i].ambi;//vec3(lights[index + 2]);
	    vec3 Diff = lights[i].diff;//vec3(lights[index + 3]);
	    vec3 Spec = lights[i].spec;//vec3(lights[index + 4]);
	    vec3 Spot = vec3(lights[i].innerTheta, lights[i].outerTheta, lights[i].spotIntensity);//vec3(lights[index + 5]);
	    vec3 attCoef = vec3(lights[i].c0, lights[i].c1, lights[i].c2);//vec3(lights[index + 5]);
	    int xtra = lights[i].type;//vec3(lights[index + 6]); // shine, type

		vec3 lightDir = Pos - defPos;
  		float dis = length(lightDir);
  		lightDir = normalize(lightDir);

  		if(xtra == 3) // Directional
  		{
  			lightDir = normalize(lights[i].dir);
  		}

		vec3 ambient = ambientStrength * Amb;
		  	
		float diff = max(dot(norm, lightDir), 0.0f);
		vec3 diffuse = Diff * diff;
		    
		//vec3 reflectDir = reflect(-lightDir, norm);  
		vec3 reflectDir = 2.0f * norm * dot(norm, lightDir) - lightDir; 
		float spec = pow(max(dot(viewDir, reflectDir), 0.0f), 32.0f);
		vec3 specular = Spec * specularStrength * spec;  

  		float att = 1.0f;

  		if(xtra != 3) // Directional
  		{
  			att = 1.0f / (attCoef.x + attCoef.y * dis + attCoef.z * (dis * dis));
  		}

  		float spotEffect = 1.0f;
  		if(xtra == 2)	// Spotlight
  		{
			float alpha = dot(lightDir, normalize(Pos)); 
			float e = cos(Spot.x) - cos(Spot.y);
			float t = alpha - cos(Spot.y);
			spotEffect = t / e;
			spotEffect = clamp(spotEffect, 0.0f, 1.0f);
			spotEffect = pow(spotEffect, Spot.z);
  		}
		        
		result += att * spotEffect * (ambient + diffuse + specular);
		//result = Amb;
	}

	float distance = length(viewPos - defPos);
	float S = (ZFar - distance) / (ZFar - ZNear);
	S = clamp(S, 0.0f, 1.0f);
	vec3 final = S * result + (1.0f - S) * fog;

	color = vec4(final, 1.0f);
}