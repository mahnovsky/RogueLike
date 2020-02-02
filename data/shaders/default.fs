#version 330 core

in vec2 UV;
in vec4 in_color;
out vec4 color;

uniform vec4 Color;

void main()
{
	color = Color;
}
