#version 330 core 

layout (location = 0) in vec3 position;
layout (location = 1) in vec4 color;
layout (location = 2) in vec2 vertexUV;

out vec2 UV;
out vec4 in_color;

//uniform mat4 MVP;
uniform mat4 ViewProjection;
uniform mat4 Model;
uniform float TimeSends;

void main()
{
	mat4 MVP = ViewProjection * Model;
    gl_Position = MVP * vec4(position, 1.0);

    in_color = color;
    UV = vertexUV;
}
