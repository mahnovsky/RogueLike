#version 330 core

in vec2 UV;
out vec4 color;

uniform sampler2D screenTexture;

uniform int width;
uniform int height;

void main()
{		
	color = texture(screenTexture, UV);
}
