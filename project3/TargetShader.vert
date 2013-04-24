/*	
	Chelsey Denton

	Code based upon code provided by Perry Kivolowitz, some code is directly copied,
	also based upon code from the OpenGL Cookbook
*/
#version 400

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_normal;
layout (location = 2) in vec3 vertex_color;

uniform mat4 mvp;
uniform mat4 modelview_matrix;
uniform mat3 normal_matrix;
uniform int seeThrough_in;

uniform vec4 Material_ADSS;

uniform vec3 light_position_1_in;
uniform vec3 light_position_2_in;
uniform vec3 light_position_3_in;
uniform int on_1_in;
uniform int on_2_in;
uniform int on_3_in;

uniform float light3_percent;

out vec3 origPosition;

out vec3 color;
out vec3 position;
out vec3 normal;
flat out uint seeThrough;
out vec4 M_ADSS;

out vec3 light_position_1;
out vec3 ADS_1;
flat out uint on_1;

out vec3 light_position_2;
out vec3 ADS_2;
flat out uint on_2;

out vec3 light_position_3;
out vec3 ADS_3;
flat out uint on_3;

void main()
{
	origPosition = vertex_position;
	seeThrough = seeThrough_in;
	color = vertex_color;
	normal = normalize(normal_matrix * vertex_normal);
	position = vec3(modelview_matrix * vec4(vertex_position,1.0));
	M_ADSS = Material_ADSS;

	light_position_1 = light_position_1_in;
	ADS_1 = vec3(.05, .8, .8);
	on_1 = on_1_in;

	light_position_2 = light_position_2_in;
	ADS_2 = vec3(.05, .8, .8);
	on_2 = on_2_in;

	light_position_3 = light_position_3_in;
	ADS_3 = vec3(.25, .01+light3_percent, light3_percent);
	on_3 = on_3_in;
	
	gl_Position = mvp * vec4(vertex_position, 1.0);
}