#version 460 core

in vec2 TexCoord;

out vec4 color;

const int L_POINT = 1;
const int L_DIR = 2;
const int L_SPOT = 3;

const int MAX_LIGHTS = 60;

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
uniform usampler2D extraTexture;

uniform int lightCount;
uniform vec3 viewPos;
uniform vec3 ambient;
uniform vec3 fog;
uniform float ZNear;
uniform float ZFar;
uniform Light lights[MAX_LIGHTS];

vec4 unpackVec(unsigned int pack)
{
	vec4 unpack = vec4(0.0f);
	unpack.x = (float(((pack >> 24)) & 0xFF) / 255.0f);
	unpack.y = (float(((pack >> 16)) & 0xFF) / 255.0f);
	unpack.z = (float(((pack >> 8)) & 0xFF) / 255.0f);
	unpack.w = (float(((pack >> 0)) & 0xFF) / 255.0f);

	return unpack;
}

void main()
{
	vec3 defPos = texture(positionTexture, TexCoord).xyz;
	vec3 defNorm = texture(normalTexture, TexCoord).xyz;
	vec3 defColor = texture(colorTexture, TexCoord).xyz;
	
	/////////////////////////////////////////////////////
	uvec4 extras = texture(extraTexture, TexCoord);
	vec2 texCoord =  extras.xy / 1000.0f;

	vec4 mColor =  unpackVec(extras.r);

	unsigned int shader =  extras.b;
	/////////////////////////////////////////////////////

	vec3 result = vec3(0.0f, 0.0f, 0.0f);
			//discard;
			//return;

	if(defNorm != vec3(0.0f, 0.0f, 0.0f))
	{
		if(shader == 0)
		{
			//color = vec4(0.0f, 1.0f, 0.0f, 1.0f);
			discard;
			return;
		}

		vec3 viewDir = normalize(viewPos - defPos);
		vec3 norm = normalize(defNorm);

		float ambientStrength = 0.1f;
		float specularStrength = 0.5f;

		result = defColor + ambientStrength * ambient;

		for(int i = 0; i < lightCount; i++)
		{
			vec3 Pos  = lights[i].pos;
			vec3 Amb  = lights[i].ambi;
			vec3 Diff = lights[i].diff;
			vec3 Spec = lights[i].spec;
			vec3 Spot = vec3(lights[i].innerTheta, lights[i].outerTheta, lights[i].spotIntensity);
			vec3 attCoef = vec3(lights[i].c0, lights[i].c1, lights[i].c2);
			int xtra = lights[i].type;

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
		    
			vec3 reflectDir = reflect(-lightDir, norm);   
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
		}

		float distance = length(viewPos - defPos);
		float S = (3600.0f - distance) / (3600.0f - ZNear);
		S = clamp(S, 0.0f, 1.0f);

		result = S * result + (1.0f - S) * fog;

		// HDR tonemapping
		result = result / (result + vec3(0.99f));
		// gamma correct
		//result = pow(result, vec3(1.0f/2.2f)); 

		color = vec4(result, 1.0f);
	}
	else
	{
		//discard;
		result = fog;
		color = vec4(result, 1.0f);
	}
}