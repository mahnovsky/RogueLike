#version 330 core

in vec2 UV;
out vec4 color;

uniform sampler2D screenTexture;

uniform int width;
uniform int height;

void main()
{
	float radius = 200.f;
	
	vec2 center = vec2(float(width) / 2, float(height) / 2);
	float a = 1.f;
	vec2 offset = center - gl_FragCoord.xy;
	float len = length(offset);
	
	a = radius / (len * 2);
	//if(len > radius ) 
	//	a = radius / (len * 2);
		
	color = vec4(texture(screenTexture, UV).xyz * a, 1.f);
}
