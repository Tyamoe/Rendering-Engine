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

uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D aoMap;

uniform int lightCount;
uniform vec3 viewPos;
uniform vec3 ambient;
uniform vec3 fog;
uniform float ZNear;
uniform float ZFar;
uniform Light lights[MAX_LIGHTS];

vec3 getNormalFromMap();
float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 fresnelSchlick(float cosTheta, vec3 F0);

void main()
{
	vec3 defPos = fragPos;//texture(positionTexture, TexCoord).xyz;
	vec3 defNorm = Normal;//texture(normalTexture, TexCoord).xyz;
	//vec3 defColor = texture(colorTexture, TexCoord).xyz;

	vec2 texCoord = TexCoord;//texture(positionTexture, TexCoord).xy;

	vec3 result = vec3(0.0f, 0.0f, 0.0f);

	if(defNorm != vec3(0.0f, 0.0f, 0.0f))
	{
		vec3 viewDir = normalize(viewPos - defPos);
		vec3 norm = normalize(defNorm);

		vec3 albedo     = pow(texture(albedoMap, TexCoords).rgb, vec3(2.2f));
		float metallic  = texture(metallicMap, TexCoords).r;
		float roughness = texture(roughnessMap, TexCoords).r;
		float ao        = texture(aoMap, TexCoords).r;

		vec3 N = getNormalFromMap();

		// calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
		// of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
		vec3 F0 = vec3(0.04f); 
		F0 = mix(F0, albedo, metallic);

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

			//result += att * spotEffect * (ambient + diffuse + specular);
			// calculate per-light radiance
			vec3 L = lightDir;//normalize(lightPositions[i] - WorldPos);
			vec3 H = normalize(viewDir + L);
			float distance = dis;//length(lightPositions[i] - WorldPos);
			float attenuation = att;//1.0 / (distance * distance);
			vec3 radiance = Diff * attenuation;

			// Cook-Torrance BRDF
			float NDF = DistributionGGX(N, H, roughness);   
			float G   = GeometrySmith(N, V, L, roughness);      
			vec3 F    = fresnelSchlick(max(dot(H, V), 0.0f), F0);
           
			vec3 nominator    = NDF * G * F; 
			float denominator = 4.0f * max(dot(N, V), 0.0f) * max(dot(N, L), 0.0f) + 0.001f; // 0.001 to prevent divide by zero.
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
			float NdotL = max(dot(N, L), 0.0f);        

			// add to outgoing radiance Lo
			result += (kD * albedo / PI + specular) * radiance * NdotL * spotEffect;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
		}

		float distance = length(viewPos - defPos);
		float S = (1600.0f - distance) / (1600.0f - ZNear);
		S = clamp(S, 0.0f, 1.0f);

		result = S * result + (1.0f - S) * fog;

		color = vec4(result, 1.0f);
	}
	else
	{
		//discard;
		result = fog;
		color = vec4(result, 1.0f);
	}
}

// ----------------------------------------------------------------------------
// Easy trick to get tangent-normals to world-space to keep PBR code simplified.
// Don't worry if you don't get what's going on; you generally want to do normal 
// mapping the usual way for performance anways; I do plan make a note of this 
// technique somewhere later in the normal mapping tutorial.
vec3 getNormalFromMap()
{
    vec3 tangentNormal = texture(normalMap, TexCoords).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(WorldPos);
    vec3 Q2  = dFdy(WorldPos);
    vec2 st1 = dFdx(TexCoords);
    vec2 st2 = dFdy(TexCoords);

    vec3 N   = normalize(Normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}
// ----------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}