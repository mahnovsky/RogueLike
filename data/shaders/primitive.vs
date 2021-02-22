#version 330 core 

layout (location = 0) in vec3 position;

//uniform mat4 MVP;
uniform mat4 ViewProjection;
uniform mat4 Model;

void main()
{
	mat4 MVP = Model * ViewProjection;
    gl_Position = MVP * vec4(position, 1.0);
}
