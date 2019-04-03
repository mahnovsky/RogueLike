#version 330 core

in vec4 in_color;
in vec2 UV;

out vec4 color;

uniform sampler2D texture_sampler;
uniform vec4 Color;

uniform vec2 offset;

void main()
{
    vec2 uv_coords = UV + offset;
    color = texture(texture_sampler, uv_coords) * Color;
}
