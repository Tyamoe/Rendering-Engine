#version 460

uniform mat4 VP;

uniform vec2 pos[200];
uniform vec2 scale;
uniform vec2 res;

layout (binding = 0, rgba32f) writeonly uniform image2D destTex;
layout (binding = 1, rgba32f) readonly uniform image2D obstacles;

layout (local_size_x = 1, local_size_y = 1) in;

float mapToRange(float val, float r1s, float r1e, float r2s, float r2e)
{
	return (val - r1s) / (r1e - r1s) * (r2e - r2s) + r2s;
}

	// In order to write to a texture, we have to introduce it as image2D.
	// local_size_x/y/z layout variables define the work group size.
	// gl_GlobalInvocationID is a uvec3 variable giving the global ID of the thread,
	// gl_LocalInvocationID is the local index within the work group, and
	// gl_WorkGroupID is the work group's index
void main()
{
    ivec2 storePos = ivec2(gl_GlobalInvocationID.xy);
    
	mat4 M = mat4(0.0f);
	M[0][0] = scale.x;
	M[1][1] = scale.y;
	M[2][2] = 1.0f;
	M[3][0] = pos[gl_GlobalInvocationID.x].x;
	M[3][1] = pos[gl_GlobalInvocationID.x].y;
	M[3][3] = 1.0f;
	
	vec4 POS = VP * M * vec4(0.0f, 0.0f, 0.0f, 1.0f);
    
    if(POS.x > 1.0f || POS.x < -1.0f)
    {
  		imageStore(destTex, storePos, vec4(0.0f, 0.0f, 0.0f, 1.0f));
        return;
    }
    else if(POS.y > 1.0f || POS.y < -1.0f)
    {
  		imageStore(destTex, storePos, vec4(0.0f, 0.0f, 0.0f, 1.0f));
        return;
    }

	float x = mapToRange(POS.x, -1.0f, 1.0f, 0.0f, res.x);
	float y = mapToRange(POS.y, -1.0f, 1.0f, 0.0f, res.y);
    
	ivec2 p = ivec2(x, y);

    vec2 obstacle = imageLoad(obstacles, p).xy;

    //float color = pos[gl_GlobalInvocationID.x].x;
    //vec2 vc = vec2(x, y) / vec2(1920.0f, 1017.0f);
    //vec4 texel1 = texture(obstacles, vc);
    //POS.x *= sign(POS.x);
    //POS.y *= sign(POS.y);

	if(obstacle.x >= 0.9f)
  {
  		//imageStore(destTex, storePos, texel);
  		//imageStore(destTex, storePos, vec4(vc.y, vc.y, 0.0f, 1.0f));
  	imageStore(destTex, storePos, vec4(obstacle.x, 0.0f, 0.0f, 1.0f));
  }
  else if(obstacle.x >= 0.5f)
  {
      //imageStore(destTex, storePos, texel);
      //imageStore(destTex, storePos, vec4(vc.y, vc.y, 0.0f, 1.0f));
    imageStore(destTex, storePos, vec4(obstacle.x, 0.0f, 0.0f, 1.0f));
  }
  else if(obstacle.x >= 0.2f)
  {
      //imageStore(destTex, storePos, texel);
      //imageStore(destTex, storePos, vec4(vc.y, vc.y, 0.0f, 1.0f));
    imageStore(destTex, storePos, vec4(obstacle.x, 0.0f, 0.0f, 1.0f));
  }
  else
  {
  		//imageStore(destTex, storePos, texel);
  		//imageStore(destTex, storePos, vec4(vc.y, vc.y, 0.0f, 1.0f));
  	imageStore(destTex, storePos, vec4(0.0f, 0.0f, 1.0f, 1.0f));
  }
}