#version 330 core

in vec4 in_color;
in vec2 UV;

out vec4 color;

uniform sampler2D texture_sampler;

void main()
{
    color = vec4( 1.0, 1.0, 1.0, texture(texture_sampler, UV).r );
}
