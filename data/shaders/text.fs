#version 330 core

in vec2 UV;

out vec4 color;

uniform sampler2D texture_sampler;
uniform vec4 Color;

void main()
{
    color = vec4( 1.0, 1.0, 1.0, texture(texture_sampler, UV).r ) * Color;
}
