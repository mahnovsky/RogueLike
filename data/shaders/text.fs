#version 330 core

in vec2 UV;

out vec4 color;

uniform sampler2D texture_sampler;
uniform vec4 Color;

void main()
{
    float alpha = texture(texture_sampler, UV).r;
    if(alpha <= 0) {
        discard;
    }

    color = vec4(Color.xyz, alpha);
}
