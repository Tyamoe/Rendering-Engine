#version 460 core

uniform vec3 oColor;

out vec4 color;

void main()
{
  color = vec4(oColor, 1.0f);
}