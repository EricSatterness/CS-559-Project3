#version 400
//Code Provided by Perry Kivolowitz
layout (location = 0) out vec4 FragColor;

in vec3 color;

uniform int hit;
void main()
{
	FragColor = vec4(color, 1.0);
	if(hit == 1)
		FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}