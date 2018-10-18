#version 330 core

in vec4 in_color;
in vec2 UV;

out vec4 color;

uniform sampler2D texture_sampler;

void main()
{
    color = texture( texture_sampler, UV ).rgba * in_color;
}
