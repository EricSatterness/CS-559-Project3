#version 400

//Code Provided by Perry Kivolowitz

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_normal;
layout (location = 2) in vec3 vertex_color;

uniform mat4 mvp;

out vec3 color;

void main()
{
	gl_Position = mvp * vec4(vertex_position, 1.0);
	color = vertex_color;
}