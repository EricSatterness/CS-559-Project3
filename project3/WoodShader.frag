/*	
	Chelsey Denton

	Code based upon code provided by Perry Kivolowitz, some code is directly copied,
	also based upon code from the OpenGL Cookbook
*/
#version 400
#define PI 3.14159
layout (location = 0) out vec4 FragColor;

uniform sampler2D Tex1;

in vec3 origPosition;
in vec3 origNormal;

in vec3 color;
in vec3 position;
in vec3 normal;
flat in uint seeThrough;
in vec4 M_ADSS;

in vec3 light_position_1;
in vec3 ADS_1;
flat in uint on_1;

in vec3 light_position_2;
in vec3 ADS_2;
flat in uint on_2;

in vec3 light_position_3;
in vec3 ADS_3;
flat in uint on_3;

void main()
{
	vec4 useColor;
	if(origNormal.z == 1)
	{
		vec2 coords = vec2(.5f + origPosition.x, .5f + origNormal.z*origPosition.y);
		useColor = texture( Tex1, coords);
	}	
	else {
		useColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	}

/*
	
	float freq = 40;
	
	float z = 0;
	float x = origPosition.x;
	float y = origPosition.y;

	int yi = 20;
	int xi = 600;

	origPosition.y += x;//sin(56*float(int(origPosition.x*xi))/xi);
	y = origPosition.y;

	x = x*xi - int(x*xi);
	x= x/xi;
	x*=4;
	x += sin(56*float(int(origPosition.x*xi))/xi);
	x += sin(99*float(int(y*yi))/yi);
	y = y*yi - float(int(y*yi));	
	y = y/2;
	y += sin((99*float(int(origPosition.y*yi))/yi)+56*float(int(origPosition.x*xi))/xi);

	float tmp = sin((x+.5)*24*freq);
	tmp += sin((x+.5)*117*freq)/40;
	tmp += sin((x+.5)*223*freq)/30;
	tmp -= 1.5;
	z = 1-(abs(y*2 - tmp)*2);

	tmp = sin((x+.5)*24*freq);
	tmp += sin((x+.5)*117*freq)/40;
	tmp += sin((x+.5)*223*freq)/30;
	tmp -= .75;
	//if(z<.1)
		z = max(1-(abs(y*2 - tmp)*4), z);

	tmp = sin((x+.5)*24*freq);
	tmp += sin((x+.5)*117*freq)/40;
	tmp += sin((x+.5)*223*freq)/30;
	tmp += .75;
	//if(z<.1)
		z = max(1-(abs(y*2 - tmp)*4), z);

	tmp = sin((x+.5)*24*freq);
	tmp += sin((x+.5)*117*freq)/40;
	tmp += sin((x+.5)*223*freq)/30;
	//if(z < .1)
		z = max(1-(abs(y*2 - tmp)*2), z);

	tmp = sin((x+.5)*24*freq);
	tmp += sin((x+.5)*117*freq)/40;
	tmp += sin((x+.5)*223*freq)/30;
	tmp += 1.5;
	//if(z < .1)
		z = max(1-(abs(y*2 - tmp)*2), z);
				
	vec3 lightBrown = vec3(.702, .349, 0);
	vec3 darkBrown = vec3(.465, .234, 0);

	z = max(0, z);

	vec3 newColor = ((lightBrown-darkBrown)*z)+darkBrown;
	
	FragColor =vec4(newColor, 1.0f);

	//FragColor =vec4(z, 0.0, 0.0, 1.0);
	*/
	
	
	float z = (origPosition.y - (sin(3 * PI * origPosition.x)/4)) * sin(3 * origPosition.x * PI);
	
	vec4 col = vec4(0.5, 0.5, 0.0, 1.0);

	z = sin(1 / z);

	col.r += z;
	col.g +=z;
	//col.b += z;
	FragColor = col;
	
}