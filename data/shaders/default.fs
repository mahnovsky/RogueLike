#version 330 core

in vec2 UV;
out vec4 color;

uniform vec4 Color;

void main()
{
    color = Color;
}
