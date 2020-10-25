#version 460 core
layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>
out vec2 TexCoords;

uniform mat4 Proj;
uniform mat4 View;

void main()
{
    gl_Position = Proj * View * vec4(vertex.xy, 0.0f, 1.0f);
    TexCoords = vertex.zw;
}  