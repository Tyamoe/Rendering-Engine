#version 460 core

in vec2 TexCoord;
out vec4 color;

uniform sampler2D texture1;

const float gamma = 1.7f;

vec3 linearToneMapping(vec3 color)
{
  float exposure = 1.;
  color = clamp(exposure * color, 0., 1.);
  color = pow(color, vec3(1. / gamma));
  return color;
}

vec3 filmicToneMapping(vec3 color)  // best
{
  color = max(vec3(0.), color - vec3(0.004));
  color = (color * (6.2 * color + .5)) / (color * (6.2 * color + 1.7) + 0.06);
  return color;
}

void main()
{
  vec3 rgb = texture(texture1, TexCoord.xy).rgb;
  rgb = filmicToneMapping(rgb);

  color = vec4(rgb, 1.0f);
	//color = texture(texture1, TexCoord);
}