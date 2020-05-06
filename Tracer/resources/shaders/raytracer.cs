#version 460

precision highp float;
precision highp int;

struct MODEL
{
    int offset;
    int triangle_count;
};

struct SPHERE
{
    vec3 pos;
    float radius;
};

struct TRIANGLE
{ 
    vec3 v0;
    vec3 v1;
    vec3 v2;
    vec3 center;
};

struct SURFACE
{
    vec3 s;
    float t;
    vec3 e;
    float r;
};

struct Ray 
{
    vec3 origin;
    vec3 direction;
};

layout (binding = 0, rgba32f) writeonly highp uniform image2D frame;

layout (std430, binding = 1) readonly buffer Spheres
{
    SPHERE spheres[];
};

layout(std430, binding = 2) readonly buffer Triangles
{
    TRIANGLE triangles[];
};

layout(std430, binding = 3) readonly buffer Surfaces
{
    SURFACE surfaces[];
};


uniform int DevI;
uniform bool DevB;
uniform float DevF;
uniform vec3 DevV;

uniform int numLights;
uniform int numSpheres;

uniform vec3 voidColor;
    
uniform vec3 CamPos;

uniform mat4 uCameraToWorld;
uniform mat4 uCameraInverseProjection;
    
uniform float uInitialSeed;

const vec3 eculer = vec3(0.8f, 0.75f, 0.15f);
const vec3 sculer = vec3(0.65f, 0.57f, 0.97f);

#define M_PI 3.14159265358979323846f
#define EPSILON 1e-3f
#define MAX_FLOAT 3.402823466e+30f

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

float rand(inout float seed, vec2 pixel)
{
    float result = fract(sin(seed / 100.0f * dot(pixel, vec2(12.9898f, 78.233f))) * 43758.5453f);
    seed += 1.0f;
    return result;
}

vec2 rand2(inout float seed, vec2 pixel)
{
    return vec2(rand(seed, pixel), rand(seed, pixel));
}

vec3 rand3(inout float seed, vec2 pixel)
{
    vec3 p;
    //do {
    p = 2.0f * vec3(rand(seed, pixel), rand(seed, pixel), rand(seed, pixel)) - vec3(1.0f);
    //} while (length(p) > 1.0f);
    return normalize(p);
}

vec3 ray_at(Ray r, float t)
{
    return r.origin + r.direction * t;
}

bool intersectTriangle(Ray r, TRIANGLE tri, out float t, out vec3 n)
{
    vec3 e1 = tri.v1 - tri.v0;
    vec3 e2 = tri.v2 - tri.v0;
    vec3 h = cross(r.direction, e2);
    float a = dot(e1, h);

    if (abs(a) < EPSILON)
        return false;

    float f = 1.0f / a;

    vec3 s = r.origin - tri.v0;

    float u = f * dot(s, h);

    if (u < 0.0f || u > 1.0f)
        return false;

    vec3 q = cross(s, e1);

    float v = f * dot(r.direction, q);

    if (v < 0.0f || u + v > 1.0f)
        return false;

    t = f * dot(e2, q);

    if (t < EPSILON)
    {
        return false;
    }

    n = normalize(cross(tri.v1 - tri.v0, tri.v2 - tri.v0));

    return true;
}

bool intersectSphere(Ray r, SPHERE sph, out float t, out vec3 n)
{
    vec3 l = sph.pos - r.origin;

    float tca = dot(l, r.direction);

    if (tca < 0)
        return false;

    float d2 = dot(l, l) - tca * tca;

    if (d2 > sph.radius * sph.radius)
        return false;

    float thc = sqrt(sph.radius * sph.radius - d2);

    t = tca - thc;
    float t1 = tca + thc;

    if (t < 0.0f)
    {
        t = t1;
    }

    vec3 phit = r.origin + r.direction * t;
    n = phit - sph.pos;

    return true;
}

bool intersetScene(Ray r, float t_min, float t_max, inout float t, inout int hit, inout vec3 n, inout vec3 c)
{
    bool does_hit = false;
    t = 0.0f;
    float best_min_t = t_max;

    hit = -1;

    bool tries = false;
    int j = 0;
    for (int i = 0; i < surfaces.length(); i++)
    {
        vec3 nor;

        if (i >= numSpheres && !tries)
        {
            tries = true;
            j = 0;
        }

        if(!tries)
        {
            SPHERE sphere = spheres[j];

            if (intersectSphere(r, sphere, t, nor) && t >= t_min && t < t_max && t < best_min_t)
            {
                best_min_t = t;
                does_hit = true;
                hit = i;
                n = nor;
                c = sphere.pos;
            }
        }
        else
        {
            TRIANGLE tri = triangles[j];

            if (intersectTriangle(r, tri, t, nor) && t >= t_min && t < t_max && t < best_min_t)
            {
                best_min_t = t;
                does_hit = true;
                hit = i;
                n = nor;

                c = tri.center;
            }
        }

        j++;
    }

    if (does_hit)
    {
        t = best_min_t;
    }

    return does_hit;
}

float Mix(float a, float b, float mix)
{
    return b * mix + a * (1.0f - mix);
}

vec3 MixV(vec3 a, vec3 b, float mix)
{
    return b * mix + a * (1.0f - mix);
}

vec3 sample_sphere_uniform(vec2 s)
{
    float phi = M_TWO_PI * s.x;
    float cos_theta = 1.0f - 2.0f * s.y;
    float sin_theta = sqrt(1.0f - cos_theta * cos_theta);

    return vec3(
        cos(phi) * sin_theta,
        cos_theta,
        sin(phi) * sin_theta);
}

/*

vec3 random_point_on_sfere(SFERE s, inout float seed, vec2 pixel, out float p)
{
    float x = rand(seed, pixel);
    float y = rand(seed, pixel);
    float z = rand(seed, pixel);

    vec3 dir = normalize(vec3(x, y, z)) * s.radius;
    dir = dir + s.pos;

    p = length(dir - (vec3(0.0f, 1.0f, 0.0f) * s.radius));

    return dir;
}

*/

#define MAX_DEPTH 1

vec3 trace_r(Ray ray)
{
    float t;
    vec3 n;
    vec3 c;
    int max_depth = MAX_DEPTH;
    int depth = 0;
    int hit;

    if (!intersetScene(ray, EPSILON, MAX_FLOAT, t, hit, n, c) || t <= 0.0f)
    {
        return voidColor;
    }

    if (hit == 0 && depth == 0)
    {
        return eculer;
    }

    vec3 nhit = normalize(n);
    vec3 phit = ray.origin + ray.direction * t;

    vec3 surfaceColor = vec3(0.0f);

    float DdotN = dot(ray.direction, nhit);
    float bias = 1e-4f;
    bool inside = false;

    if (DdotN > 0.0f)
    {
        inside = true;

        nhit = -nhit;
        DdotN = dot(ray.direction, nhit);
    }

    // Phong Step
    vec3 lightDir = spheres[0].pos - phit;
    lightDir = normalize(lightDir);

    float diff = max(dot(nhit, lightDir), 0.0f);
    vec3 diffuse = diff * surfaces[0].e;

    float specularStrength = 0.5f;
    vec3 viewDir = normalize(CamPos - phit);
    vec3 reflectDir = reflect(-lightDir, nhit);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), 32.0f);
    vec3 specular = specularStrength * spec * surfaces[0].e;

    surfaceColor = (diffuse + specular) * surfaces[hit].s;

    // Shadow Step
    for (int i = 0; i < numLights; i++)
    {
        vec3 trans = vec3(1.0f);

        vec3 lightDir1 = spheres[i].pos - phit;
        lightDir1 = normalize(lightDir1);

        Ray sr;
        sr.origin = phit + nhit * bias;
        sr.direction = lightDir1;
        int shadowInd = 0;

        vec3 sn;
        vec3 sc;

        if (intersetScene(sr, EPSILON, MAX_FLOAT, t, shadowInd, sn, sc) && shadowInd >= numLights)
        {
            trans = vec3(0.0f);
        }

        surfaceColor = surfaceColor * trans;
    }
    
    return surfaceColor;
}

vec3 trace(Ray ray, inout float seed, vec2 pixel)
{
    float t;
    vec3 n;
    vec3 c;
    int max_depth = MAX_DEPTH;
    int depth = 0;
    int hit;

    vec3 res = vec3(0.0f);

    while (depth < max_depth)
    {
        if(!intersetScene(ray, EPSILON, MAX_FLOAT, t, hit, n, c) || t <= 0.0f)
        {
            if(depth == 0)
            {
                return voidColor;
            }

            break;
        }

        if (hit == 0 && depth == 0)
        {
            return eculer;
        }

        vec3 nhit = normalize(n);

        vec3 phit = ray.origin + ray.direction * t;

        vec3 surfaceColor = vec3(0.0f);

        if (hit != 0)
        {
            float DdotN = dot(ray.direction, nhit);
            float bias = 1e-4f;
            bool inside = false;

            if (DdotN > 0.0f)
            {
                inside = true;

                nhit = -nhit;
                DdotN = dot(ray.direction, nhit);
            }
            bool refion = false;

            vec3 incomingColor = vec3(0.0f);

            // Reflection Step
            if (surfaces[hit].r > 0.0f || surfaces[hit].t > 0.0f)
            {
                vec3 refldir = ray.direction - nhit * 2.0f * DdotN;
                refldir = normalize(refldir);

                // Compute r color
                Ray r;
                r.origin = phit + nhit * bias;
                r.direction = refldir;
                vec3 reflect = trace_r(r);

                vec3 refract = vec3(0.0f);

                float facingratio = -DdotN;
                float fresEffect = Mix(pow(1.0f - facingratio, 3.0f), 1.0f, 0.1f);

                // Refraction Step
                if (surfaces[hit].t > 0.0f)
                {
                    float ior = 0.4f, eta = (inside) ? ior : 1.0f / ior;
                    float cosi = -dot(nhit, ray.direction);

                    float k = 1.0f - eta * eta * (1.0f - cosi * cosi);

                    // Compute refraction direction
                    vec3 refrdir = ray.direction * eta + nhit * (eta * cosi - sqrt(k));
                    refrdir = normalize(refrdir);

                    // Compute refraction color
                    Ray rr;
                    rr.origin = phit - nhit * bias;
                    rr.direction = refrdir;
                    refract = trace_r(rr);
                }

                incomingColor = (reflect * fresEffect + refract * (1.0f - fresEffect) * surfaces[hit].t) * surfaces[hit].s;
            }

            // Phong Step
            vec3 lightDir = spheres[0].pos - phit;
            lightDir = normalize(lightDir);

            float diff = max(dot(nhit, lightDir), 0.0f);
            vec3 diffuse = diff * surfaces[0].e;

            float specularStrength = 0.5f;
            vec3 viewDir = normalize(CamPos - phit);
            vec3 reflectDir = reflect(-lightDir, nhit);
            float spec = pow(max(dot(viewDir, reflectDir), 0.0f), 32.0f);
            vec3 specular = specularStrength * spec * surfaces[0].e;

            surfaceColor = (diffuse + specular) * surfaces[hit].s;

            // Mix Step
            float mixVal = clamp(surfaces[hit].r + surfaces[hit].t, 0.0f, 1.0f);
            surfaceColor = MixV(surfaceColor, incomingColor, mixVal);

            // Shadow Step
            for (int i = 0; i < numLights; i++)
            {
                vec3 trans = vec3(1.0f);

                vec3 lightDir1 = spheres[i].pos - phit;
                lightDir1 = normalize(lightDir1);

                Ray sr;
                sr.origin = phit + nhit * bias;
                sr.direction = lightDir1;
                int shadowInd = 0;

                vec3 sn;
                vec3 sc;

                if (intersetScene(sr, EPSILON, MAX_FLOAT, t, shadowInd, sn, sc) && shadowInd >= numLights)
                {
                    trans = vec3(0.0f);
                }

                surfaceColor = surfaceColor * trans;
            }
        }

        res += surfaceColor;
        depth++;
    }

    return res / depth;
}

Ray create_camera_ray(vec2 uv, vec2 dim)
{
    //int width = 800;
    //int height = 600;

    float fov = 75.0f;

    float aspect = dim.x / dim.y;
    float angle = tan(M_PI * 0.5f * fov / 180.0f);

    float invWidth = 1.0f / dim.x;
    float invHeight = 1.0f / dim.y;

    vec3 origin = (uCameraToWorld * vec4(0.0f, 0.0f, 0.0f, 1.0f)).xyz;

    //vec3 direction = (uCameraInverseProjection * vec4(uv, 0.0f, 1.0f)).xyz;
    //direction = (uCameraToWorld * vec4(direction.xy, -1.0f, 0.0f)).xyz;
    //direction = normalize(direction);

    float xx = (2.0f * ((uv.x + 0.5f) * invWidth) - 1.0f) * aspect * angle;
    float yy = (1.0f - 2.0f * ((uv.y + 0.5f) * invHeight)) * angle;

    // Ray direction considering camera
    vec3 direction = (uCameraToWorld * vec4(xx, yy, -1.0f, 1.0f)).xyz;
    direction = direction - origin;
    direction = normalize(direction);

    Ray res;
    res.origin = origin;
    res.direction = direction;
    return res;
}

void main() 
{
    // gl_LocalInvocationId: local index of the worker in its group.
    // gl_WorkGroupId : index of the current working group.
    // gl_WorkGroupSize : local size of a work group. here it is 16x16x1.
    // gl_GlovalInvocationId : global exectuion index of the current worker.
    // gl_GlobalInvocationId = gl_WorkGroupID * gl_WorkGroupSize + gl_LocalInvocationID

    ivec2 storePos = ivec2(gl_GlobalInvocationID.xy);
    ivec2 imageSize = ivec2(gl_NumWorkGroups.xy * gl_WorkGroupSize.xy);
    vec2 uv = vec2(storePos) / vec2(imageSize);
    float seed = uInitialSeed;

    vec2 sample_pos = (vec2(storePos) + rand2(seed, uv)) / vec2(imageSize);

    vec2 xy = vec2(storePos.x, imageSize.y - storePos.y);
    // Generate a camera ray.
    Ray r = create_camera_ray(xy, vec2(imageSize));

    // Shade the sample.
    vec3 finalColor = trace(r, seed, uv);

    //vec3 ccc = normalize(sferes[0].pos);
    // Merge this sample with previous samples.
    vec4 color1 = vec4(finalColor, 1.0f);

    // Write the pixel.
    imageStore(frame, storePos, color1);
}