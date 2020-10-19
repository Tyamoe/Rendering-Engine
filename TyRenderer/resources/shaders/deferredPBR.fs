#version 460 core

in vec3 fragPos;
//in vec4 Normal;
in vec2 TexCoord;

out vec4 color;

const int L_POINT = 1;
const int L_DIR = 2;
const int L_SPOT = 3;

const int MAX_LIGHTS = 60;

const float PI = 3.14159265359f;

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

//uniform sampler2D albedoMap;
//uniform sampler2D normalMap;
//uniform sampler2D metallicMap;
//uniform sampler2D roughnessMap;
//uniform sampler2D aoMap;

uniform sampler2D positionTexture; // albedoMap
uniform sampler2D normalTexture; // normal
uniform sampler2D colorTexture; // met, rough, ao
uniform usampler2D extraTexture; // texcoords

uniform int lightCount;
uniform vec3 viewPos;
uniform vec3 ambient;
uniform vec3 fog;
uniform float ZNear;
uniform float ZFar;
uniform Light lights[MAX_LIGHTS];

float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 fresnelSchlick(float cosTheta, vec3 F0);

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

	vec3 defNormal = texture(normalTexture, TexCoord).xyz;
	vec3 defAlbedo = texture(colorTexture, TexCoord).xyz;

	vec4 defMRA = vec4(0.0f);

	/////////////////////////////////////////////////////
	uvec4 extras = texture(extraTexture, TexCoord);

	unsigned int shader =  extras.b;
	unsigned int mra =  extras.r;

	defMRA = unpackVec(mra);

	/////////////////////////////////////////////////////

	vec3 result = vec3(0.0f, 0.0f, 0.0f);

	if(defNormal != vec3(0.0f, 0.0f, 0.0f))
	{
		if(shader == 1)
		{
			//color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
			discard;
			return;
		}

		vec3 viewDir = normalize(viewPos - defPos);
		vec3 norm = normalize(defNormal);

		vec3 albedo     = pow(defAlbedo, vec3(2.2f));//pow(texture(albedoMap, TexCoords).rgb, vec3(2.2f));
		float metallic  = defMRA.r;
		float roughness = defMRA.g;
		float ao        = defMRA.b;

		// calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
		// of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
		vec3 F0 = vec3(0.04f); 
		F0 = mix(F0, albedo, metallic);

		for(int i = 0; i < lightCount; i++)
		{
			vec3 attCoef = vec3(lights[i].c0, lights[i].c1, lights[i].c2);

			vec3 lightDir = lights[i].pos - defPos;
  			float distance = length(lightDir);
  			lightDir = normalize(lightDir);

  			if(lights[i].type == 3) // Directional
  			{
  				lightDir = normalize(lights[i].dir);
  			}

  			float att = 1.0f;
  			if(lights[i].type != 3) // Directional
  			{
  				att = 1.0f / (attCoef.x + attCoef.y * distance + attCoef.z * (distance * distance));
  			}

  			float spotEffect = 1.0f;
  			if(lights[i].type == 2)	// Spotlight
  			{
				float alpha = dot(lightDir, normalize(lights[i].pos)); 
				float e = cos(lights[i].innerTheta) - cos(lights[i].outerTheta);
				float t = alpha - cos(lights[i].outerTheta);
				spotEffect = t / e;
				spotEffect = clamp(spotEffect, 0.0f, 1.0f);
				spotEffect = pow(spotEffect, lights[i].spotIntensity);
  			}

			// calculate per-light radiance
			vec3 H = normalize(viewDir + lightDir);
			vec3 radiance = lights[i].ambi * att;

			// Cook-Torrance BRDF
			float NDF = DistributionGGX(defNormal, H, roughness);   
			float G   = GeometrySmith(defNormal, viewDir, lightDir, roughness);      
			vec3 F    = fresnelSchlick(max(dot(H, viewDir), 0.0f), F0);
           
			vec3 nominator    = NDF * G * F; 
			float denominator = 4.0f * max(dot(defNormal, viewDir), 0.0f) * max(dot(defNormal, lightDir), 0.0f) + 0.001f; // 0.001 to prevent divide by zero.
			vec3 specular = nominator / denominator;
        
			// kS is equal to Fresnel
			vec3 kS = F;
			// for energy conservation, the diffuse and specular light can't
			// be above 1.0 (unless the surface emits light); to preserve this
			// relationship the diffuse component (kD) should equal 1.0 - kS.
			vec3 kD = vec3(1.0f) - kS;
			// multiply kD by the inverse metalness such that only non-metals 
			// have diffuse lighting, or a linear blend if partly metal (pure metals
			// have no diffuse light).
			kD *= 1.0f - metallic;	  

			// scale light by NdotL
			float NdotL = max(dot(defNormal, lightDir), 0.0f);        

			// add to outgoing radiance Lo
			result += (kD * albedo / PI + specular) * radiance * NdotL * spotEffect;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
		}

		vec3 ambient11 = vec3(0.03f) * albedo * ao;
    	result += ambient11;
	
    	result = result / (result + vec3(1.0f));
    	result = pow(result, vec3(1.0f/2.2f));

		float distance = length(viewPos - defPos);
		float S = (3337.0f - distance) / (3337.0f - ZNear);
		S = clamp(S, 0.0f, 1.0f);

		result = S * result + (1.0f - S) * fog;



		//color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
		color = vec4(vec3(result), 1.0f);
	}
	else
	{
		//discard;
		result = fog;
		color = vec4(result, 1.0f);
	}
}

// ----------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0f);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0f) + 1.0f);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0f);
    float k = (r*r) / 8.0f;

    float nom   = NdotV;
    float denom = NdotV * (1.0f - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0f);
    float NdotL = max(dot(N, L), 0.0f);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0f - F0) * pow(1.0f - cosTheta, 5.0f);
}