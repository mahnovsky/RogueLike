#version 330 core

in vec4 in_color;
in vec2 UV;

out vec4 color;

uniform sampler2D texture_sampler;
uniform vec4 Color;

void main()
{
    color = texture(texture_sampler, UV) * Color;
}
